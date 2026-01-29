#include "factory.hxx"
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <map>

enum class NodeColor {
    UNVISITED,
    VISITED,
    VERIFIED
};

bool has_reachable_storehouse(const IPackageSender* sender, std::map<const IPackageSender*, NodeColor>& node_colors) {
    
    if (node_colors[sender] == NodeColor::VERIFIED) {
        return true;
    }

    node_colors[sender] = NodeColor::VISITED;

    if (sender->receiver_preferences_.get_preferences().empty()) {
        throw std::logic_error("Sender has no receivers defined.");
    }

    bool does_sender_have_reachable_storehouse = false; 

    for (const auto& [receiver, probability] : sender->receiver_preferences_.get_preferences()) {
        
        if (receiver->get_receiver_type() == ReceiverType::STOREHOUSE) {
            does_sender_have_reachable_storehouse = true;
        } 
        else if (receiver->get_receiver_type() == ReceiverType::WORKER) {
            IPackageReceiver* receiver_ptr = receiver;
            auto worker_ptr = dynamic_cast<Worker*>(receiver_ptr);
            auto sendrecv_ptr = dynamic_cast<const IPackageSender*>(worker_ptr);

            if (sendrecv_ptr == sender) {
                continue;
            }

            does_sender_have_reachable_storehouse = true; 

            if (node_colors[sendrecv_ptr] == NodeColor::UNVISITED) {
                has_reachable_storehouse(sendrecv_ptr, node_colors); 
            }
        }
    }

    node_colors[sender] = NodeColor::VERIFIED;

    if (does_sender_have_reachable_storehouse) {
        return true;
    } else {
        throw std::logic_error("Sender has no reachable storehouse.");
    }
}



template <typename Sender>
void Factory::remove_receiver_links(NodeCollection<Sender>& collection, IPackageReceiver* receiver) {
    for (auto& sender : collection) {
        sender.receiver_preferences_.remove_receiver(receiver);
    }
}

void Factory::remove_worker(ElementID id) {
    auto it = workers_.find_by_id(id);
    if (it != workers_.end()) {
        Worker* w = &(*it);
        remove_receiver_links(ramps_, w);
        remove_receiver_links(workers_, w);
        workers_.remove_by_id(id);
    }
}

void Factory::remove_storehouse(ElementID id) {
    auto it = storehouses_.find_by_id(id);
    if (it != storehouses_.end()) {
        Storehouse* s = &(*it);
        remove_receiver_links(ramps_, s);
        remove_receiver_links(workers_, s);
        storehouses_.remove_by_id(id);
    }
}

bool Factory::is_consistent() {
    std::map<const IPackageSender*, NodeColor> node_colors;
    for (const auto& ramp : ramps_) {
        node_colors[&ramp] = NodeColor::UNVISITED;
    }
    for (const auto& worker : workers_) {
        node_colors[&worker] = NodeColor::UNVISITED;
    }
    try {
        for (const auto& ramp : ramps_) {
            has_reachable_storehouse(&ramp, node_colors);
        }
    } catch (const std::logic_error&) {
        return false;
    }
    return true;
    
}

void Factory::do_deliveries(Time t) {
    for (auto& ramp : ramps_) {
        ramp.deliver_goods(t);
    }
}

void Factory::do_work(Time t) {
    for (auto& worker : workers_) {
        worker.do_work(t);
    }
}

void Factory::do_package_passing(Time t) {
    for (auto& ramp : ramps_) {
        if (ramp.get_sending_buffer()) {
            ramp.send_package();
        }
    }
    for (auto& worker : workers_) {
         if (worker.get_sending_buffer()) {
            worker.send_package();
        }
    }
}

std::pair<std::string, std::string> parse_key_value(const std::string& token) {
    auto pos = token.find('=');
    if (pos == std::string::npos) return {token, ""};
    return {token.substr(0, pos), token.substr(pos + 1)};
}

Factory load_factory_structure(std::istream& is) {
    Factory factory;
    std::string line;
    
    while (std::getline(is, line)) {
        if (line.empty() || line[0] == ';') continue; 
        
        std::istringstream iss(line);
        std::string type;
        iss >> type;
        
        if (type == "LOADING_RAMP") {
            std::string token;
            ElementID id = 0;
            TimeOffset di = 1;
            while (iss >> token) {
                auto [key, val] = parse_key_value(token);
                if (key == "id") id = std::stoi(val);
                else if (key == "delivery-interval") di = std::stoi(val);
            }
            factory.add_ramp(Ramp(id, di));
        } else if (type == "WORKER") {
            std::string token;
            ElementID id = 0;
            TimeOffset pd = 1;
            PackageQueueType qt = PackageQueueType::FIFO;
            while (iss >> token) {
                auto [key, val] = parse_key_value(token);
                if (key == "id") id = std::stoi(val);
                else if (key == "processing-time") pd = std::stoi(val);
                else if (key == "queue-type") qt = (val == "LIFO") ? PackageQueueType::LIFO : PackageQueueType::FIFO;
            }
            factory.add_worker(Worker(id, pd, std::make_unique<PackageQueue>(qt)));
        } else if (type == "STOREHOUSE") {
            std::string token;
            ElementID id = 0;
            while (iss >> token) {
                auto [key, val] = parse_key_value(token);
                if (key == "id") id = std::stoi(val);
            }
            factory.add_storehouse(Storehouse(id));
        } else if (type == "LINK") {
            std::string token;
            std::string src_val, dest_val;
            while (iss >> token) {
                auto [key, val] = parse_key_value(token);
                if (key == "src") src_val = val;
                else if (key == "dest") dest_val = val;
            }
            
            auto parse_node_id = [](const std::string& s) -> std::pair<std::string, ElementID> {
                auto pos = s.find('-');
                return {s.substr(0, pos), std::stoi(s.substr(pos + 1))};
            };
            
            auto [src_type, src_id] = parse_node_id(src_val);
            auto [dest_type, dest_id] = parse_node_id(dest_val);

            IPackageSender* sender = nullptr;
            if (src_type == "ramp") sender = &(*factory.find_ramp_by_id(src_id));
            else if (src_type == "worker") sender = &(*factory.find_worker_by_id(src_id));

            IPackageReceiver* receiver = nullptr;
            if (dest_type == "worker") receiver = &(*factory.find_worker_by_id(dest_id));
            else if (dest_type == "store") receiver = &(*factory.find_storehouse_by_id(dest_id));

            if (sender && receiver) {
                sender->receiver_preferences_.add_receiver(receiver);
            }
        }
    }
    return factory;
}

void save_factory_structure(const Factory& factory, std::ostream& os) {
    for (auto it = factory.ramp_cbegin(); it != factory.ramp_cend(); ++it) {
        os << "LOADING_RAMP id=" << it->get_id() << " delivery-interval=" << it->get_delivery_interval() << "\n";
    }

    for (auto it = factory.worker_cbegin(); it != factory.worker_cend(); ++it) {
        std::string q_type = (it->get_queue()->get_queue_type() == PackageQueueType::LIFO) ? "LIFO" : "FIFO";
        os << "WORKER id=" << it->get_id() << " processing-time=" << it->get_processing_duration() << " queue-type=" << q_type << "\n";
    }

    for (auto it = factory.storehouse_cbegin(); it != factory.storehouse_cend(); ++it) {
        os << "STOREHOUSE id=" << it->get_id() << "\n";
    }

    for (auto it = factory.ramp_cbegin(); it != factory.ramp_cend(); ++it) {
        for (const auto& [receiver, prob] : it->receiver_preferences_.get_preferences()) {
             std::string dest_type = (receiver->get_receiver_type() == ReceiverType::WORKER) ? "worker" : "store";
             os << "LINK src=ramp-" << it->get_id() << " dest=" << dest_type << "-" << receiver->get_id() << "\n";
        }
    }

    for (auto it = factory.worker_cbegin(); it != factory.worker_cend(); ++it) {
        for (const auto& [receiver, prob] : it->receiver_preferences_.get_preferences()) {
             std::string dest_type = (receiver->get_receiver_type() == ReceiverType::WORKER) ? "worker" : "store";
             os << "LINK src=worker-" << it->get_id() << " dest=" << dest_type << "-" << receiver->get_id() << "\n";
        }
    }
}
