#ifndef PACKAGE_HXX_
#define PACKAGE_HXX_

#include "types.hxx"
#include <set>
#include <utility>

class Package {
public:
    Package();
    explicit Package(ElementID id);
    
    Package(Package&& other) noexcept;
    Package& operator=(Package&& other) noexcept;

    ElementID get_id() const { return id_; }

    ~Package();

    Package(const Package&) = delete;
    Package& operator=(const Package&) = delete;

private:
    ElementID id_;
    static std::set<ElementID> freed_ids_;
    static ElementID assigned_ids_;
};

#endif