#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

constexpr std::uintmax_t MaxFileBytes = 50ULL * 1024ULL * 1024ULL;

struct FileRecord {
    std::string name;
    std::string category;
    std::string source;
    std::string destination;
    std::uintmax_t size = 0;
};

class FileManager {
private:
    fs::path dataDirectory;
    fs::path inboxDirectory;
    fs::path organizedDirectory;

public:
    explicit FileManager(fs::path directory);

    bool saveUploadedFile(
        const std::string& filename,
        const std::string& content,
        std::string& savedName,
        std::string& error
    );

    std::vector<fs::path> listInboxFiles() const;

    std::vector<fs::path> listOrganizedFiles() const;

    const fs::path& inboxPath() const;

    const fs::path& organizedPath() const;

    void ensureCategoryDirectories(
        const std::vector<std::string>& categories
    ) const;

    static std::string safeFilename(
        const std::string& filename
    );
};