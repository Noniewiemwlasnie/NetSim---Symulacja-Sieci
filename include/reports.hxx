#ifndef REPORTS_HXX_
#define REPORTS_HXX_

#include "factory.hxx"
#include "types.hxx"
#include <set>
#include <iostream>

class SpecificTurnsReportNotifier {
public:
    SpecificTurnsReportNotifier(std::set<Time> turns) : turns_(std::move(turns)) {}
    bool should_generate_report(Time t) { return turns_.find(t) != turns_.end(); }
private:
    std::set<Time> turns_;
};

class IntervalReportNotifier {
public:
    IntervalReportNotifier(TimeOffset to) : to_(to) {}
    bool should_generate_report(Time t) { return (t - 1) % to_ == 0; }
private:
    TimeOffset to_;
};

void generate_structure_report(const Factory& f, std::ostream& os);

void generate_simulation_turn_report(const Factory& f, std::ostream& os, Time t);

#endif