/*!
 * @file SimpleIterant.cpp
 * @date 12 Aug 2021
 * @author Tim Spain <timothy.spain@nersc.no>
 */

#include "SimpleIterant.hpp"

#include <ctime>
#include <iostream>

std::string stringFromTimePoint(const Nextsim::Iterator::TimePoint&);

namespace Nextsim {

SimpleIterant::SimpleIterant()
{
    // It's so simple, there's nothing here
}

void SimpleIterant::init(const Environment& env)
{
    std::cout << "SimpleIterant::init" << std::endl;
}

void SimpleIterant::start(const Iterator::TimePoint& startTime)
{
    std::cout << "SimpleIterant::start at " << stringFromTimePoint(startTime) << std::endl;
}

void SimpleIterant::iterate(const Iterator::Duration& dt)
{
    std::cout << "SimpleIterant::iterate for " << dt.count() << std::endl;
}

void SimpleIterant::stop(const Iterator::TimePoint& stopTime)
{
    std::cout << "SimpleIterant::stop at " << stringFromTimePoint(stopTime) << std::endl;
}

} /* namespace Nextsim */

std::string stringFromTimePoint(const Nextsim::Iterator::TimePoint& t)
{
    std::time_t t_c = Nextsim::Iterator::Clock::to_time_t(t);
    return std::string(ctime(&t_c));
}
