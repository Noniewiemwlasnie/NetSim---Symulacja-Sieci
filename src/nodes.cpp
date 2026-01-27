#include "nodes.hxx"

Storehouse::Storehouse(ElementID id, std::unique_ptr<IPackageStockpile> d) 
    : id_(id), d_(std::move(d)) {}

void Storehouse::receive_package(Package&& p) {
    d_->push(std::move(p));
}

ReceiverPreferences::ReceiverPreferences(ProbabilityGenerator pg) : pg_(pg) {}

void ReceiverPreferences::add_receiver(IPackageReceiver* receiver) {
    preferences_[receiver] = 1.0;
    double p = 1.0 / preferences_.size();
    for (auto& item : preferences_) {
        item.second = p;
    }
}

void ReceiverPreferences::remove_receiver(IPackageReceiver* receiver) {
    preferences_.erase(receiver);
    if (!preferences_.empty()) {
        double p = 1.0 / preferences_.size();
        for (auto& item : preferences_) {
            item.second = p;
        }
    }
}

IPackageReceiver* ReceiverPreferences::choose_receiver() {
    double p = pg_(); 
    double distribution = 0.0;
    for (const auto& item : preferences_) {
        distribution += item.second;
        if (p <= distribution) {
            return item.first;
        }
    }
    if (!preferences_.empty()) return preferences_.rbegin()->first;
    return nullptr;
}

void IPackageSender::push_package(Package&& package) {
    IPackageReceiver* receiver = receiver_preferences_.choose_receiver();
    if (receiver) {
        receiver->receive_package(std::move(package));
    }
}

void IPackageSender::send_package() {
    if (get_sending_buffer()) {
        push_package(std::move(get_sending_buffer().value()));
        get_sending_buffer().reset();
    }
}

Ramp::Ramp(ElementID id, TimeOffset di) : id_(id), di_(di) {}

void Ramp::deliver_goods(Time t) {
    if ((t - 1) % di_ == 0) {
        if (!buffer_) {
             buffer_ = Package();
        }
    }
}

Worker::Worker(ElementID id, TimeOffset pd, std::unique_ptr<IPackageStockpile> q)
    : id_(id), pd_(pd), q_(std::move(q)) {}

void Worker::receive_package(Package&& p) {
    q_->push(std::move(p));
}

void Worker::do_work(Time t) {
    if (!processing_buffer_ && !q_->empty()) {
        processing_buffer_ = q_->pop();
        t_ = t;
    }
    
    if (processing_buffer_) {
        if (t - t_ + 1 >= pd_) {
            sending_buffer_ = std::move(processing_buffer_);
            processing_buffer_.reset();
        }
    }
}