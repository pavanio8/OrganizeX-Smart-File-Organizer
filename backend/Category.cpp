#include "Category.h"
#include <algorithm>
#include <cctype>
#include <utility>

Category::Category(std::string name)
    : name(std::move(name)) {}

const std::string& Category::getName() const {
    return name;
}

ExtensionCategory::ExtensionCategory(
    std::string name,
    std::vector<std::string> extensions
)
    : Category(std::move(name)),
      extensions(std::move(extensions)) {

    for (auto& ext : this->extensions) {
        std::transform(
            ext.begin(),
            ext.end(),
            ext.begin(),
            [](unsigned char c) {
                return std::tolower(c);
            }
        );
    }
}

bool ExtensionCategory::matches(
    const std::string& extension
) const {
    return std::find(
        extensions.begin(),
        extensions.end(),
        extension
    ) != extensions.end();
}

OtherCategory::OtherCategory()
    : Category("Others") {}

bool OtherCategory::matches(const std::string&) const {
    return true;
}

CategoryManager::CategoryManager()
    : other(std::make_unique<OtherCategory>()) {
    categories.emplace_back(std::make_unique<ExtensionCategory>(
        "Documents",
        std::vector<std::string>{
            "pdf", "doc", "docx", "txt", "ppt", "pptx", "xls", "xlsx"}));
    categories.emplace_back(std::make_unique<ExtensionCategory>(
        "Images",
        std::vector<std::string>{
            "jpg", "jpeg", "png", "gif", "svg", "webp"}));
    categories.emplace_back(std::make_unique<ExtensionCategory>(
        "Videos",
        std::vector<std::string>{
            "mp4", "mkv", "avi", "mov", "wmv"}));
    categories.emplace_back(std::make_unique<ExtensionCategory>(
        "Audio",
        std::vector<std::string>{
            "mp3", "wav", "aac", "flac"}));
    categories.emplace_back(std::make_unique<ExtensionCategory>(
        "Archives",
        std::vector<std::string>{
            "zip", "rar", "7z", "tar", "gz"}));
    categories.emplace_back(std::make_unique<ExtensionCategory>(
        "Code",
        std::vector<std::string>{
            "cpp", "h", "hpp", "c", "java", "py", "js", "html", "css"}));
}

const Category& CategoryManager::classify(
    const std::string& filename
) const {

    size_t dot = filename.find_last_of('.');

    std::string ext =
        dot == std::string::npos
        ? ""
        : filename.substr(dot + 1);

    std::transform(
        ext.begin(),
        ext.end(),
        ext.begin(),
        [](unsigned char c) {
            return std::tolower(c);
        }
    );

    for (const auto& category : categories) {
        if (category->matches(ext)) {
            return *category;
        }
    }

    return *other;
}

std::vector<std::string>
CategoryManager::getCategoryNames() const {

    std::vector<std::string> result;

    for (const auto& category : categories) {
        result.push_back(category->getName());
    }

    result.push_back(other->getName());

    return result;
}