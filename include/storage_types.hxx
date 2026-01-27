#ifndef STORAGE_TYPES_HXX_
#define STORAGE_TYPES_HXX_

#include <list>
#include <optional>
#include "package.hxx"

enum class PackageQueueType {
    FIFO,
    LIFO
};

class IPackageStockpile {
public:
    using const_iterator = std::list<Package>::const_iterator;

    virtual void push(Package&& package) = 0;
    virtual bool empty() const = 0;
    virtual std::size_t size() const = 0;
    virtual std::optional<Package> pop() = 0; 
    virtual PackageQueueType get_queue_type() const = 0;
    virtual ~IPackageStockpile() = default;
    
    virtual const_iterator cbegin() const = 0;
    virtual const_iterator cend() const = 0;
    virtual const_iterator begin() const = 0;
    virtual const_iterator end() const = 0;
};

using IPackageQueue = IPackageStockpile;

class PackageQueue : public IPackageStockpile {
public:
    explicit PackageQueue(PackageQueueType type) : queue_type_(type) {}

    void push(Package&& package) override;
    bool empty() const override;
    std::size_t size() const override;
    std::optional<Package> pop() override;
    
    PackageQueueType get_queue_type() const override { return queue_type_; }

    const_iterator cbegin() const override { return queue_.cbegin(); }
    const_iterator cend() const override { return queue_.cend(); }
    const_iterator begin() const override { return queue_.begin(); }
    const_iterator end() const override { return queue_.end(); }

private:
    std::list<Package> queue_;
    PackageQueueType queue_type_;
};

#endif