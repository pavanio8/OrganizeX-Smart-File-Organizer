#include "Category.h"
#include "FileManager.h"
#include "Organizer.h"

#include "httplib.h"
#include <nlohmann/json.hpp>

#include <filesystem>
#include <exception>
#include <iostream>
#include <mutex>
#include <string>

using json = nlohmann::json;
namespace fs = std::filesystem;

namespace {
constexpr size_t MaxRequestBytes = 100ULL * 1024ULL * 1024ULL;
constexpr size_t MaxFilesPerRequest = 100;

bool isAllowedOrigin(const std::string& origin) {
    return origin == "http://127.0.0.1:5500" ||
        origin == "http://localhost:5500";
}

void setJson(httplib::Response& response, const json& body) {
    response.set_content(body.dump(), "application/json; charset=utf-8");
}
}

int main() {
    try {
        const fs::path dataFolder =
            fs::current_path() / "OrganizeX_Data";
        FileManager fileManager(dataFolder);
        CategoryManager categoryManager;
        fileManager.ensureCategoryDirectories(
            categoryManager.getCategoryNames());
        Organizer organizer(fileManager, categoryManager);
        std::mutex filesystemMutex;
        httplib::Server server;

        server.set_payload_max_length(MaxRequestBytes);

        server.set_pre_routing_handler(
            [](const httplib::Request& request, httplib::Response& response) {
                const std::string origin =
                    request.get_header_value("Origin");
                if (isAllowedOrigin(origin)) {
                    response.set_header("Access-Control-Allow-Origin",
                        origin.c_str());
                    response.set_header("Vary", "Origin");
                    response.set_header("Access-Control-Allow-Methods",
                        "GET, POST, OPTIONS");
                    response.set_header("Access-Control-Allow-Headers",
                        "Content-Type");
                }
                if (request.method == "OPTIONS") {
                    response.status = 204;
                    return httplib::Server::HandlerResponse::Handled;
                }
                return httplib::Server::HandlerResponse::Unhandled;
            });

        server.set_error_handler(
            [](const httplib::Request&, httplib::Response& response) {
                const int status = response.status;
                setJson(response, {
                    {"ok", false},
                    {"error", status == 413
                        ? "Request exceeds the 100 MB upload limit."
                        : "The requested API route was not found."}
                });
            });

        server.set_exception_handler(
            [](const httplib::Request&, httplib::Response& response,
               std::exception_ptr exception) {
                std::string message = "An unexpected server error occurred.";
                try {
                    if (exception) {
                        std::rethrow_exception(exception);
                    }
                } catch (const std::exception& error) {
                    std::cerr << "Request error: " << error.what() << '\n';
                } catch (...) {
                    std::cerr << "Request error: unknown exception\n";
                }
                response.status = 500;
                setJson(response, {{"ok", false}, {"error", message}});
            });

        server.Get("/api/status",
            [](const httplib::Request&, httplib::Response& response) {
                setJson(response, {
                    {"ok", true},
                    {"status", "online"},
                    {"message", "OrganizeX backend is connected."}
                });
            });

        server.Post("/api/upload",
            [&](const httplib::Request& request,
                httplib::Response& response) {
                if (!request.is_multipart_form_data()) {
                    response.status = 400;
                    setJson(response, {
                        {"ok", false},
                        {"error", "Expected a multipart/form-data upload."}
                    });
                    return;
                }

                json results = json::array();
                json errors = json::array();
                size_t fileCount = 0;
                size_t uploaded = 0;
                std::lock_guard<std::mutex> lock(filesystemMutex);

                for (const auto& item : request.files) {
                    if (item.first != "files") {
                        continue;
                    }
                    const size_t fileIndex = fileCount;
                    const std::string safeName =
                        FileManager::safeFilename(item.second.filename);
                    if (++fileCount > MaxFilesPerRequest) {
                        errors.push_back({
                            {"index", fileIndex},
                            {"name", safeName},
                            {"error", "Upload is limited to 100 files per request."}
                        });
                        continue;
                    }

                    std::string savedName;
                    std::string error;
                    if (fileManager.saveUploadedFile(
                            item.second.filename, item.second.content,
                            savedName, error)) {
                        ++uploaded;
                        results.push_back({
                            {"index", fileIndex},
                            {"name", safeName},
                            {"savedName", savedName},
                            {"success", true}
                        });
                    } else {
                        errors.push_back({
                            {"index", fileIndex},
                            {"name", safeName},
                            {"error", error}
                        });
                    }
                }

                if (fileCount == 0) {
                    response.status = 400;
                    setJson(response, {
                        {"ok", false},
                        {"error", "No files were supplied in the 'files' field."}
                    });
                    return;
                }

                setJson(response, {
                    {"ok", errors.empty()},
                    {"uploaded", uploaded},
                    {"results", results},
                    {"errors", errors}
                });
            });

        server.Get("/api/preview",
            [&](const httplib::Request&, httplib::Response& response) {
                std::lock_guard<std::mutex> lock(filesystemMutex);
                json rows = json::array();
                for (const auto& file : organizer.preview()) {
                    rows.push_back({
                        {"name", file.name},
                        {"category", file.category},
                        {"size", file.size},
                        {"destination", file.destination}
                    });
                }
                setJson(response, {{"files", rows}, {"count", rows.size()}});
            });

        server.Post("/api/organize",
            [&](const httplib::Request&, httplib::Response& response) {
                std::lock_guard<std::mutex> lock(filesystemMutex);
                const auto results = organizer.organize();
                json operations = json::array();
                size_t moved = 0;
                for (const auto& result : results) {
                    operations.push_back({
                        {"name", result.name},
                        {"destination", result.destination},
                        {"success", result.success},
                        {"error", result.error}
                    });
                    if (result.success) {
                        ++moved;
                    }
                }
                setJson(response, {
                    {"success", moved == results.size()},
                    {"moved", moved},
                    {"failed", results.size() - moved},
                    {"results", operations}
                });
            });

        server.Get("/api/statistics",
            [&](const httplib::Request&, httplib::Response& response) {
                std::lock_guard<std::mutex> lock(filesystemMutex);
                json categories = json::object();
                for (const auto& name : categoryManager.getCategoryNames()) {
                    categories[name] = 0;
                }

                std::uintmax_t totalBytes = 0;
                size_t pendingFiles = 0;
                size_t organizedFiles = 0;
                const auto countFiles = [&](const std::vector<fs::path>& files) {
                    for (const auto& path : files) {
                        const std::string category =
                            categoryManager.classify(path.filename().string())
                                .getName();
                        categories[category] =
                            categories.value(category, 0) + 1;
                        std::error_code error;
                        const auto size = fs::file_size(path, error);
                        if (!error) {
                            totalBytes += size;
                        }
                    }
                };

                const auto pending = fileManager.listInboxFiles();
                const auto organized = fileManager.listOrganizedFiles();
                pendingFiles = pending.size();
                organizedFiles = organized.size();
                countFiles(pending);
                countFiles(organized);

                setJson(response, {
                    {"totalFiles", pendingFiles + organizedFiles},
                    {"pendingFiles", pendingFiles},
                    {"organizedFiles", organizedFiles},
                    {"categoryCount", categoryManager.getCategoryNames().size()},
                    {"totalBytes", totalBytes},
                    {"categories", categories}
                });
            });

        std::cout << "OrganizeX server running at http://127.0.0.1:8080\n"
                  << "Files are stored in the local OrganizeX_Data folder.\n";
        if (!server.listen("127.0.0.1", 8080)) {
            std::cerr << "Server failed to start on 127.0.0.1:8080.\n";
            return 1;
        }
    } catch (const std::exception& error) {
        std::cerr << "Could not start OrganizeX: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
