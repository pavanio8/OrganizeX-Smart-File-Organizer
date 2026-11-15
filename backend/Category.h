#pragma once

#include <string>
#include <vector>
#include <memory>

class Category {
protected:
    std::string name;

    explicit Category(std::string name);

public:
    virtual ~Category() = default;

    const std::string& getName() const;

    virtual bool matches(const std::string& extension) const = 0;
};

class ExtensionCategory final : public Category {
private:
    std::vector<std::string> extensions;

public:
    ExtensionCategory(
        std::string name,
        std::vector<std::string> extensions
    );

    bool matches(const std::string& extension) const override;
};

class OtherCategory final : public Category {
public:
    OtherCategory();

    bool matches(const std::string& extension) const override;
};

class CategoryManager {
private:
    std::vector<std::unique_ptr<Category>> categories;
    std::unique_ptr<Category> other;

public:
    CategoryManager();

    const Category& classify(
        const std::string& filename
    ) const;

    std::vector<std::string> getCategoryNames() const;
};