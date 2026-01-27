#include "package.hxx"

std::set<ElementID> Package::freed_ids_;
ElementID Package::assigned_ids_ = 1;

Package::Package() {
    if (freed_ids_.empty()) {
        id_ = assigned_ids_++;
    } else {
        id_ = *freed_ids_.begin();
        freed_ids_.erase(freed_ids_.begin());
    }
}

Package::Package(ElementID id) : id_(id) {
    if (id >= assigned_ids_) {
        assigned_ids_ = id + 1;
    }
}

Package::Package(Package&& other) noexcept : id_(other.id_) {
    other.id_ = -1; 
}

Package& Package::operator=(Package&& other) noexcept {
    if (this != &other) {
        if (id_ != -1) {
            freed_ids_.insert(id_);
        }
        id_ = other.id_;
        other.id_ = -1;
    }
    return *this;
}

Package::~Package() {
    if (id_ != -1) {
        freed_ids_.insert(id_);
    }
}