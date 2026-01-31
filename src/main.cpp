#include <iostream>
#include <fstream>
#include <string>

#include "factory.hxx"
#include "simulation.hxx"
#include "reports.hxx"

int main() {
    std::cout << "Symulacja Fabryki - Start" << std::endl;

    Factory factory;

    factory.add_ramp(Ramp(1, 2));
    factory.add_worker(Worker(1, 2, std::make_unique<PackageQueue>(PackageQueueType::FIFO))); 
    factory.add_storehouse(Storehouse(1)); 

    factory.find_ramp_by_id(1)->receiver_preferences_.add_receiver(&(*factory.find_worker_by_id(1)));
    factory.find_worker_by_id(1)->receiver_preferences_.add_receiver(&(*factory.find_storehouse_by_id(1)));

    auto reporter = [](Factory& f, Time t) {
        generate_simulation_turn_report(f, std::cout, t);
    };

    try {
        simulate(factory, 10, reporter);
    } catch (const std::exception& e) {
        std::cerr << "Blad symulacji: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Symulacja zakonczona." << std::endl;
    return 0;
}