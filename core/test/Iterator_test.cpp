/*!
 * @file Iterator_test.cpp
 * @date 12 Aug 2021
 * @author Tim Spain <timothy.spain@nersc.no>
 */

#include "Iterator.hpp"

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

namespace Nextsim {

// An iterant that counts the number of times it is started, iterated
// and stopped
class Counterant : public Iterator::Iterant {
public:
    void init()
    {
        count = 0;
        startCount = 0;
        stopCount = 0;
    };
    void start(const TimePoint& startTime) { startCount++; };
    void iterate(const TimestepTime& tst) { count++; };
    void stop(const TimePoint& stopTime) { stopCount++; };

    int getCount() { return count; };

    int count;
    int startCount;
    int stopCount;
};

template<typename T>
T zeroTime();

template<>
std::chrono::time_point<Iterator::Clock> zeroTime<std::chrono::time_point<Iterator::Clock>>()
{
    return Iterator::Clock::now();
}
template<>
int zeroTime<int>()
{
    return 0;
}


TEST_CASE("Count iterator testing", "[Iterator]")
{
    Counterant cant = Counterant();
    Iterator iterator = Iterator(&cant);

    int nSteps = 5;

    TimePoint start = zeroTime<TimePoint>();
    Duration dt = 1;
    iterator.setStartStopStep(start, start + nSteps * dt, dt);
    iterator.run();

    REQUIRE(cant.count == nSteps);
    REQUIRE(cant.startCount == 1);
    REQUIRE(cant.stopCount == 1);
}

} /* namespace Nextsim */
