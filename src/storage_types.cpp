#include "storage_types.hxx"

void PackageQueue::push(Package&& package) {
    queue_.emplace_back(std::move(package));
}

bool PackageQueue::empty() const {
    return queue_.empty();
}

std::size_t PackageQueue::size() const {
    return queue_.size();
}

std::optional<Package> PackageQueue::pop() {
    if (empty()) {
        return std::nullopt;
    }
    
    Package p = std::move((queue_type_ == PackageQueueType::FIFO) ? queue_.front() : queue_.back());
    
    if (queue_type_ == PackageQueueType::FIFO) {
        queue_.pop_front();
    } else {
        queue_.pop_back();
    }
    
    return std::move(p);
}