#pragma once

#include "Category.h"
#include "FileManager.h"

struct OrganizationResult {
    std::string name;
    std::string destination;
    bool success = false;
    std::string error;
};

class Organizer {
private:
    FileManager& fileManager;
    const CategoryManager& categoryManager;

    std::string uniqueDestination(
        const fs::path& directory,
        const std::string& filename
    );

public:
    Organizer(
        FileManager& fm,
        const CategoryManager& cm
    );

    std::vector<FileRecord> preview() const;

    std::vector<OrganizationResult> organize();
};
