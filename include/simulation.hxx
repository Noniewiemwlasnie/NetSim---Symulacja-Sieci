#ifndef SIMULATION_HXX_
#define SIMULATION_HXX_

#include "factory.hxx"
#include "types.hxx"
#include <functional>

void simulate(Factory& f, TimeOffset rounds, std::function<void(Factory&, Time)> rf);

#endif