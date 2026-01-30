#include "reports.hxx"
#include <algorithm>

void generate_structure_report(const Factory& f, std::ostream& os) {
    save_factory_structure(f, os);
}

void generate_simulation_turn_report(const Factory& f, std::ostream& os, Time t) {
    os << "=== [ Turn: " << t << " ] ===\n";

    os << "WORKERS_BLUEPRINT\n"; 
    for (auto it = f.worker_cbegin(); it != f.worker_cend(); ++it) {
        const auto& worker = *it;
        os << "WORKER #" << worker.get_id() << "\n";

        const auto& processing_buffer = worker.get_processing_buffer();
        
        if (processing_buffer) {
            os << "  PBuffer: #" << processing_buffer->get_id() << " (pt = " 
               << (t - worker.get_package_processing_start_time() + 1) << ")\n";
        } else {
            os << "  PBuffer: (empty)\n";
        }

        os << "  Queue: ";
        const auto* queue = worker.get_queue();
        if (queue->empty()) {
            os << "(empty)";
        } else {
            for (const auto& pkg : *queue) {
                os << "#" << pkg.get_id() << " ";
            }
        }
        os << "\n";
    }

    os << "STOREHOUSES_BLUEPRINT\n";
    for (auto it = f.storehouse_cbegin(); it != f.storehouse_cend(); ++it) {
        const auto& store = *it;
        os << "STOREHOUSE #" << store.get_id() << "\n";
        os << "  Stock: ";
        if (store.cbegin() == store.cend()) {
            os << "(empty)";
        } else {
            for (auto pkg_it = store.cbegin(); pkg_it != store.cend(); ++pkg_it) {
                os << "#" << pkg_it->get_id() << " ";
            }
        }
        os << "\n";
    }
    
    os << "\n";
}