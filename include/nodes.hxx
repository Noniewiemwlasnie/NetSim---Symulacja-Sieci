#ifndef NODES_HXX_
#define NODES_HXX_

#include <memory>
#include <map>
#include <optional>
#include "types.hxx"
#include "package.hxx"
#include "storage_types.hxx"
#include "helpers.hxx"

enum class ReceiverType {
    WORKER,
    STOREHOUSE
};

class IPackageReceiver {
public:
    virtual void receive_package(Package&& p) = 0;
    virtual ElementID get_id() const = 0;
    virtual ReceiverType get_receiver_type() const = 0;
    
    using const_iterator = IPackageStockpile::const_iterator;
    virtual const_iterator cbegin() const = 0;
    virtual const_iterator cend() const = 0;
    virtual const_iterator begin() const = 0;
    virtual const_iterator end() const = 0;

    virtual ~IPackageReceiver() = default;
};

class Storehouse : public IPackageReceiver {
public:
    Storehouse(ElementID id, std::unique_ptr<IPackageStockpile> d = std::make_unique<PackageQueue>(PackageQueueType::FIFO));
    
    void receive_package(Package&& p) override;
    ElementID get_id() const override { return id_; }
    ReceiverType get_receiver_type() const override { return ReceiverType::STOREHOUSE; }

    const_iterator cbegin() const override { return d_->cbegin(); }
    const_iterator cend() const override { return d_->cend(); }
    const_iterator begin() const override { return d_->begin(); }
    const_iterator end() const override { return d_->end(); }

private:
    ElementID id_;
    std::unique_ptr<IPackageStockpile> d_;
};

class ReceiverPreferences {
public:
    using preferences_t = std::map<IPackageReceiver*, double>;
    using const_iterator = preferences_t::const_iterator;

    ReceiverPreferences(ProbabilityGenerator pg = default_probability_generator);
    
    void add_receiver(IPackageReceiver* receiver);
    void remove_receiver(IPackageReceiver* receiver);
    IPackageReceiver* choose_receiver();
    const preferences_t& get_preferences() const { return preferences_; }

    const_iterator begin() const { return preferences_.begin(); }
    const_iterator end() const { return preferences_.end(); }
    const_iterator cbegin() const { return preferences_.cbegin(); }
    const_iterator cend() const { return preferences_.cend(); }

private:
    ProbabilityGenerator pg_;
    preferences_t preferences_;
};

class IPackageSender {
public:
    IPackageSender(ReceiverPreferences preferences = ReceiverPreferences()) 
        : receiver_preferences_(std::move(preferences)) {}

    void send_package();
    virtual std::optional<Package>& get_sending_buffer() = 0;
    
    ReceiverPreferences receiver_preferences_; 
    
    virtual ~IPackageSender() = default;
protected:
    void push_package(Package&& package);
};

class Ramp : public IPackageSender {
public:
    Ramp(ElementID id, TimeOffset di);
    
    void deliver_goods(Time t);
    TimeOffset get_delivery_interval() const { return di_; }
    ElementID get_id() const { return id_; }
    
    std::optional<Package>& get_sending_buffer() override { return buffer_; }

private:
    ElementID id_;
    TimeOffset di_;
    std::optional<Package> buffer_;
};

class Worker : public IPackageReceiver, public IPackageSender {
public:
    Worker(ElementID id, TimeOffset pd, std::unique_ptr<IPackageStockpile> q);
    
    void receive_package(Package&& p) override;
    ElementID get_id() const override { return id_; }
    ReceiverType get_receiver_type() const override { return ReceiverType::WORKER; }

    void do_work(Time t);
    
    TimeOffset get_processing_duration() const { return pd_; }
    Time get_package_processing_start_time() const { return t_; }
    
    // Metoda wymagana przez testy
    IPackageStockpile* get_queue() const { return q_.get(); }

    std::optional<Package>& get_sending_buffer() override { return sending_buffer_; }

    const_iterator cbegin() const override { return q_->cbegin(); }
    const_iterator cend() const override { return q_->cend(); }
    const_iterator begin() const override { return q_->begin(); }
    const_iterator end() const override { return q_->end(); }

private:
    ElementID id_;
    TimeOffset pd_; 
    Time t_ = 0;    
    
    std::unique_ptr<IPackageStockpile> q_;
    std::optional<Package> processing_buffer_;
    std::optional<Package> sending_buffer_;
};

#endif