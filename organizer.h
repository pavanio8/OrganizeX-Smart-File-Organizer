#ifndef ORGANIZER_H
#define ORGANIZER_H

#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

struct FileItem {
    fs::path source;
    fs::path destination;
    std::string category;
    bool conflict = false;
};

struct OrganizeResult {
    int moved = 0;
    int skipped = 0;
    std::vector<std::string> messages;
};

class Organizer {
public:
    std::vector<FileItem> preview(const fs::path& directory) const;

    OrganizeResult organize(const fs::path& directory) const;

private:
    std::string categoryFor(const fs::path& file) const;
};

#endif