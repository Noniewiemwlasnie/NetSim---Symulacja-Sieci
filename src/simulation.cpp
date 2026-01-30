#include "simulation.hxx"
#include <stdexcept>

void simulate(Factory& f, TimeOffset rounds, std::function<void(Factory&, Time)> rf) {
    if (!f.is_consistent()) {
        throw std::logic_error("Network is inconsistent");
    }

    for (Time t = 1; t <= rounds; ++t) {

        f.do_deliveries(t);

        f.do_package_passing(t);

        f.do_work(t);

        rf(f, t);
    }
}