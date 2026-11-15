#include "FileManager.h"

#include <fstream>
#include <cctype>
#include <algorithm>
#include <stdexcept>
#include <utility>

FileManager::FileManager(fs::path directory)
    : dataDirectory(std::move(directory)) {

    fs::create_directories(dataDirectory);
    std::error_code statusError;
    const auto dataStatus = fs::symlink_status(dataDirectory, statusError);
    if (statusError || dataStatus.type() != fs::file_type::directory) {
        throw std::runtime_error(
            "OrganizeX_Data must be a real directory, not a file or symbolic link.");
    }
    dataDirectory = fs::canonical(dataDirectory);

    inboxDirectory = dataDirectory / "Inbox";
    organizedDirectory = dataDirectory / "Organized";

    fs::create_directories(inboxDirectory);
    fs::create_directories(organizedDirectory);
    const auto inboxStatus = fs::symlink_status(inboxDirectory, statusError);
    if (statusError || inboxStatus.type() != fs::file_type::directory) {
        throw std::runtime_error("The OrganizeX Inbox must be a real directory.");
    }
    const auto organizedStatus =
        fs::symlink_status(organizedDirectory, statusError);
    if (statusError || organizedStatus.type() != fs::file_type::directory) {
        throw std::runtime_error(
            "The OrganizeX Organized folder must be a real directory.");
    }
}

std::string FileManager::safeFilename(
    const std::string& filename
) {
    const std::string name = fs::path(filename).filename().string();
    std::string clean;

    for (unsigned char c : name) {
        if (std::isalnum(c) || c == '.' || c == '-' ||
            c == '_' || c == ' ') {
            clean += static_cast<char>(c);
        }
    }

    while (!clean.empty() &&
           (clean.front() == '.' || clean.front() == ' ')) {
        clean.erase(clean.begin());
    }

    while (!clean.empty() &&
           (clean.back() == '.' || clean.back() == ' ')) {
        clean.pop_back();
    }

    if (clean.empty() || clean == "." || clean == "..") {
        return "";
    }

    if (clean.size() > 180) {
        const fs::path path(clean);
        const std::string extension = path.extension().string();
        const size_t maxStem = 180 - std::min<size_t>(extension.size(), 32);
        clean = path.stem().string().substr(0, maxStem) +
            extension.substr(0, 32);
    }

    std::string stem = fs::path(clean).stem().string();
    std::transform(stem.begin(), stem.end(), stem.begin(),
        [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    const std::vector<std::string> reservedNames = {
        "CON", "PRN", "AUX", "NUL",
        "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9",
        "LPT1", "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9"
    };
    if (std::find(reservedNames.begin(), reservedNames.end(), stem) !=
        reservedNames.end()) {
        clean.insert(0, "_");
    }

    return clean;
}

bool FileManager::saveUploadedFile(
    const std::string& filename,
    const std::string& content,
    std::string& savedName,
    std::string& error
) {
    if (content.size() > MaxFileBytes) {
        error = "File exceeds the 50 MB per-file limit.";
        return false;
    }

    savedName = safeFilename(filename);
    if (savedName.empty()) {
        error = "The filename is empty or invalid.";
        return false;
    }

    const fs::path original(savedName);
    fs::path target = inboxDirectory / original;
    int index = 1;
    while (fs::exists(target)) {
        target = inboxDirectory / (
            original.stem().string() + "_" + std::to_string(index++) +
            original.extension().string());
    }
    savedName = target.filename().string();

    std::ofstream file(target, std::ios::binary);

    if (!file) {
        error = "Cannot create file.";
        return false;
    }

    file.write(
        content.data(),
        static_cast<std::streamsize>(content.size())
    );

    if (!file.good()) {
        error = "Cannot write file.";
        file.close();

        std::error_code ec;
        fs::remove(target, ec);

        return false;
    }

    return true;
}

std::vector<fs::path>
FileManager::listInboxFiles() const {

    std::vector<fs::path> files;

    for (const auto& entry :
         fs::directory_iterator(inboxDirectory)) {
        std::error_code ec;
        if (entry.symlink_status(ec).type() == fs::file_type::regular) {
            files.push_back(entry.path());
        }
    }

    return files;
}

std::vector<fs::path> FileManager::listOrganizedFiles() const {
    std::vector<fs::path> files;
    std::error_code ec;
    if (!fs::exists(organizedDirectory, ec) || ec) {
        return files;
    }

    for (const auto& category : fs::directory_iterator(organizedDirectory)) {
        std::error_code categoryError;
        if (category.symlink_status(categoryError).type() !=
            fs::file_type::directory) {
            continue;
        }
        for (const auto& entry : fs::directory_iterator(category.path())) {
            std::error_code entryError;
            if (entry.symlink_status(entryError).type() ==
                fs::file_type::regular) {
                files.push_back(entry.path());
            }
        }
    }
    return files;
}

void FileManager::ensureCategoryDirectories(
    const std::vector<std::string>& categories
) const {
    for (const auto& category : categories) {
        const std::string safeCategory = safeFilename(category);
        if (safeCategory.empty() || safeCategory != category) {
            throw std::runtime_error("Invalid category directory name.");
        }
        const fs::path directory = organizedDirectory / safeCategory;
        fs::create_directories(directory);
        std::error_code ec;
        if (fs::symlink_status(directory, ec).type() !=
                fs::file_type::directory || ec) {
            throw std::runtime_error(
                "A category folder must be a real directory.");
        }
    }
}

const fs::path& FileManager::inboxPath() const {
    return inboxDirectory;
}

const fs::path& FileManager::organizedPath() const {
    return organizedDirectory;
}