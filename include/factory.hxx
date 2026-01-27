#ifndef FACTORY_HXX_
#define FACTORY_HXX_

#include <vector>
#include <string>
#include <algorithm>
#include <list>
#include <istream>
#include "nodes.hxx"

template <class Node>
class NodeCollection {
public:
    using container_t = std::list<Node>;
    using iterator = typename container_t::iterator;
    using const_iterator = typename container_t::const_iterator;

    void add(Node&& node) { nodes_.emplace_back(std::move(node)); }
    
    void remove_by_id(ElementID id) {
        auto it = find_by_id(id);
        if (it != nodes_.end()) {
            nodes_.erase(it);
        }
    }

    iterator find_by_id(ElementID id) {
        return std::find_if(nodes_.begin(), nodes_.end(), 
            [id](const Node& node){ return node.get_id() == id; });
    }

    const_iterator find_by_id(ElementID id) const {
        return std::find_if(nodes_.begin(), nodes_.end(), 
            [id](const Node& node){ return node.get_id() == id; });
    }

    iterator begin() { return nodes_.begin(); }
    iterator end() { return nodes_.end(); }
    const_iterator cbegin() const { return nodes_.cbegin(); }
    const_iterator cend() const { return nodes_.cend(); }
    const_iterator begin() const { return nodes_.begin(); }
    const_iterator end() const { return nodes_.end(); }

private:
    container_t nodes_;
};

class Factory {
public:
    void add_ramp(Ramp&& ramp) { ramps_.add(std::move(ramp)); }
    void remove_ramp(ElementID id) { ramps_.remove_by_id(id); }
    NodeCollection<Ramp>& ramp_collection() { return ramps_; } 

    NodeCollection<Ramp>::iterator ramp_begin() { return ramps_.begin(); }
    NodeCollection<Ramp>::iterator ramp_end() { return ramps_.end(); }
    NodeCollection<Ramp>::const_iterator ramp_cbegin() const { return ramps_.cbegin(); }
    NodeCollection<Ramp>::const_iterator ramp_cend() const { return ramps_.cend(); }
    NodeCollection<Ramp>::const_iterator ramp_begin() const { return ramps_.begin(); }
    NodeCollection<Ramp>::const_iterator ramp_end() const { return ramps_.end(); }

    void add_worker(Worker&& worker) { workers_.add(std::move(worker)); }
    void remove_worker(ElementID id);
    NodeCollection<Worker>& worker_collection() { return workers_; }

    NodeCollection<Worker>::iterator worker_begin() { return workers_.begin(); }
    NodeCollection<Worker>::iterator worker_end() { return workers_.end(); }
    NodeCollection<Worker>::const_iterator worker_cbegin() const { return workers_.cbegin(); }
    NodeCollection<Worker>::const_iterator worker_cend() const { return workers_.cend(); }
    NodeCollection<Worker>::const_iterator worker_begin() const { return workers_.begin(); }
    NodeCollection<Worker>::const_iterator worker_end() const { return workers_.end(); }

    void add_storehouse(Storehouse&& storehouse) { storehouses_.add(std::move(storehouse)); }
    void remove_storehouse(ElementID id);
    NodeCollection<Storehouse>& storehouse_collection() { return storehouses_; }

    NodeCollection<Storehouse>::iterator storehouse_begin() { return storehouses_.begin(); }
    NodeCollection<Storehouse>::iterator storehouse_end() { return storehouses_.end(); }
    NodeCollection<Storehouse>::const_iterator storehouse_cbegin() const { return storehouses_.cbegin(); }
    NodeCollection<Storehouse>::const_iterator storehouse_cend() const { return storehouses_.cend(); }
    NodeCollection<Storehouse>::const_iterator storehouse_begin() const { return storehouses_.begin(); }
    NodeCollection<Storehouse>::const_iterator storehouse_end() const { return storehouses_.end(); }

    NodeCollection<Ramp>::iterator find_ramp_by_id(ElementID id) { return ramps_.find_by_id(id); }
    NodeCollection<Worker>::iterator find_worker_by_id(ElementID id) { return workers_.find_by_id(id); }
    NodeCollection<Storehouse>::iterator find_storehouse_by_id(ElementID id) { return storehouses_.find_by_id(id); }
    
    NodeCollection<Ramp>::const_iterator find_ramp_by_id(ElementID id) const { return ramps_.find_by_id(id); }
    NodeCollection<Worker>::const_iterator find_worker_by_id(ElementID id) const { return workers_.find_by_id(id); }
    NodeCollection<Storehouse>::const_iterator find_storehouse_by_id(ElementID id) const { return storehouses_.find_by_id(id); }

    bool is_consistent();
    void do_deliveries(Time t);
    void do_package_passing(Time t);
    void do_work(Time t);

private:
    NodeCollection<Ramp> ramps_;
    NodeCollection<Worker> workers_;
    NodeCollection<Storehouse> storehouses_;
    
    template <typename Sender>
    void remove_receiver_links(NodeCollection<Sender>& collection, IPackageReceiver* receiver);
};

Factory load_factory_structure(std::istream& is);
void save_factory_structure(const Factory& factory, std::ostream& os);

#endif