#include "organizer.h"

#include <algorithm>
#include <cctype>
#include <unordered_map>

std::string Organizer::categoryFor(const fs::path& file) const {

    std::string ext = file.extension().string();

    // Convert the extension to lowercase.
    std::transform(
        ext.begin(),
        ext.end(),
        ext.begin(),
        [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        }
    );

    static const std::unordered_map<std::string, std::string> categories = {

        // Documents
        {".pdf", "Documents"},
        {".doc", "Documents"},
        {".docx", "Documents"},
        {".txt", "Documents"},
        {".ppt", "Documents"},
        {".pptx", "Documents"},
        {".xls", "Documents"},
        {".xlsx", "Documents"},

        // Images
        {".jpg", "Images"},
        {".jpeg", "Images"},
        {".png", "Images"},
        {".gif", "Images"},
        {".bmp", "Images"},
        {".svg", "Images"},
        {".webp", "Images"},

        // Videos
        {".mp4", "Videos"},
        {".mkv", "Videos"},
        {".avi", "Videos"},
        {".mov", "Videos"},

        // Audio
        {".mp3", "Audio"},
        {".wav", "Audio"},
        {".aac", "Audio"},
        {".flac", "Audio"},

        // Archives
        {".zip", "Archives"},
        {".rar", "Archives"},
        {".7z", "Archives"},
        {".tar", "Archives"},
        {".gz", "Archives"},

        // Programming files
        {".cpp", "Code"},
        {".c", "Code"},
        {".h", "Code"},
        {".hpp", "Code"},
        {".java", "Code"},
        {".py", "Code"},
        {".js", "Code"},
        {".html", "Code"},
        {".css", "Code"}
    };

    auto it = categories.find(ext);

    if (it != categories.end()) {
        return it->second;
    }

    return "Others";
}


// Preview files without moving them.
std::vector<FileItem> Organizer::preview(
    const fs::path& directory
) const {

    std::vector<FileItem> items;

    if (!fs::exists(directory) || !fs::is_directory(directory)) {
        return items;
    }

    std::error_code ec;

    for (
        fs::directory_iterator it(directory, ec), end;
        !ec && it != end;
        it.increment(ec)
    ) {

        const auto& entry = *it;

        std::error_code typeError;

        if (!entry.is_regular_file(typeError) || typeError) {
            continue;
        }

        FileItem item;

        item.source = entry.path();
        item.category = categoryFor(entry.path());

        item.destination =
            directory / item.category / entry.path().filename();

        item.conflict = fs::exists(item.destination);

        items.push_back(item);
    }

    std::sort(
        items.begin(),
        items.end(),
        [](const FileItem& a, const FileItem& b) {
            return a.source.filename().string()
                 < b.source.filename().string();
        }
    );

    return items;
}


// Organize files into folders.
OrganizeResult Organizer::organize(
    const fs::path& directory
) const {

    OrganizeResult result;

    if (!fs::exists(directory) || !fs::is_directory(directory)) {
        result.messages.push_back("Invalid directory.");
        return result;
    }

    // Create the preview list before moving any files.
    const auto items = preview(directory);

    for (const auto& item : items) {

        try {

            // Never overwrite an existing destination file.
            if (item.conflict || fs::exists(item.destination)) {

                result.skipped++;

                result.messages.push_back(
                    "Skipped: " + item.source.filename().string()
                );

                continue;
            }

            // Create the category folder.
            fs::create_directories(
                item.destination.parent_path()
            );

            // Move the file.
            fs::rename(
                item.source,
                item.destination
            );

            result.moved++;

            result.messages.push_back(
                "Moved: " + item.source.filename().string()
            );

        }
        catch (const fs::filesystem_error& error) {

            result.skipped++;

            result.messages.push_back(
                "Error: " + std::string(error.what())
            );
        }
    }

    return result;
}