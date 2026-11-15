#include "Organizer.h"
#include <filesystem>
#include <system_error>
#include <utility>

Organizer::Organizer(
    FileManager& fm,
    const CategoryManager& cm
)
    : fileManager(fm),
      categoryManager(cm) {}

std::vector<FileRecord> Organizer::preview() const {

    std::vector<FileRecord> records;

    for (const auto& path : fileManager.listInboxFiles()) {

        const auto& category =
            categoryManager.classify(path.filename().string());

        FileRecord record;

        record.name = path.filename().string();
        record.category = category.getName();
        record.source = path.string();
        record.destination = "Organized/" + record.category + "/" + record.name;

        std::error_code ec;

        record.size = fs::file_size(path, ec);

        records.push_back(record);
    }

    return records;
}

std::string Organizer::uniqueDestination(
    const fs::path& directory,
    const std::string& filename
) {
    fs::path target = directory / filename;

    if (!fs::exists(target)) {
        return target.string();
    }

    fs::path original(filename);

    int index = 1;

    do {
        target = directory /
            (original.stem().string() +
             "_" + std::to_string(index++) +
             original.extension().string());

    } while (fs::exists(target));

    return target.string();
}

std::vector<OrganizationResult> Organizer::organize() {
    std::vector<OrganizationResult> results;

    for (const auto& record : preview()) {
        fs::path source(record.source);
        const fs::path folder = fileManager.organizedPath() / record.category;
        std::error_code ec;
        fs::create_directories(folder, ec);

        OrganizationResult result;
        result.name = record.name;
        result.destination = record.destination;
        if (ec) {
            result.error = "Could not create the category folder.";
            results.push_back(std::move(result));
            continue;
        }
        if (fs::symlink_status(folder, ec).type() !=
                fs::file_type::directory || ec) {
            result.error = "The category destination is not a safe directory.";
            results.push_back(std::move(result));
            continue;
        }

        const fs::path destination(
            uniqueDestination(folder, record.name));
        fs::rename(source, destination, ec);
        if (ec) {
            result.error = "Could not move this file; it remains in Inbox.";
        } else {
            result.success = true;
            result.destination = "Organized/" + record.category + "/" +
                destination.filename().string();
        }
        results.push_back(std::move(result));
    }

    return results;
}