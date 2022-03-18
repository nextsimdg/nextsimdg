/*!
 * @file ModelData_test.cpp
 *
 * @date Feb 24, 2022
 * @author Tim Spain <timothy.spain@nersc.no>
 */

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "../src/include/ModelArray.hpp"

namespace Nextsim {

TEST_CASE("Two dimensional data access test", "[ModelArray]")
{
    ModelArray::Dimensions dims2 = {15, 25};

    ModelArray::setDimensions(ModelArray::Type::H, dims2);

    ModelArray check1d = ModelArray::HField("check1d");

    REQUIRE(check1d.nDimensions() == 2);

    // Check the ordering: later indexes should vary faster
    // Fill data as 1d
    for (size_t i = 0; i < dims2[0] * dims2[1]; ++i) {
        check1d[i] = i;
    }

    size_t x = 7;
    size_t y = 13;
    // Check neighbouring y indices differ in value by 1
    REQUIRE(check1d(x, y) - check1d(x, y-1) == 1);
    // Check neighbouring x values differ in value by ny
    REQUIRE(check1d(x, y) - check1d(x-1, y) == dims2[1]);

    REQUIRE(check1d(dims2[0]-1, dims2[1]-1) == dims2[0] * dims2[1] - 1);
}

TEST_CASE("Higher dimensional indexing", "[ModelArray]")
{
    size_t dimLen = 10;
    size_t arrayLen = dimLen * dimLen * dimLen * dimLen;
    ModelArray::Dimensions dims4 = {dimLen, dimLen, dimLen, dimLen};
    ModelArray::setDimensions(ModelArray::Type::H, dims4);

    ModelArray check4d = ModelArray::HField("check4d");

    REQUIRE(check4d.nDimensions() == 4);
    REQUIRE(check4d.size() == dimLen * dimLen * dimLen * dimLen);

    for (size_t i = 0; i < check4d.size(); ++i) {
        check4d[i] = i;
    }

    // Check indexing using the fact that the dimensions are the same as our counting base
    REQUIRE(check4d(4, 7, 2, 5) == 4725);

    // Reset the data to zero
    for (size_t i = 0; i < check4d.size(); ++i) {
        check4d[i] = 0.;
    }
    REQUIRE(check4d(4, 7, 2, 5) == 0);

    double* data = new double[arrayLen];
    for (size_t i = 0; i < arrayLen; ++i) {
        data[i] = i;
    }

    check4d.setData(data);

    REQUIRE(check4d(4, 7, 2, 5) == 4725);

    // Reset the data to zero
    for (size_t i = 0; i < check4d.size(); ++i) {
        check4d[i] = 0.;
    }
    REQUIRE(check4d(4, 7, 2, 5) == 0);

    std::vector<double> vData = std::vector<double>(arrayLen);
    for (size_t i = 0; i < arrayLen; ++i) {
        vData[i] = i;
    }

    check4d.setData(vData);

    REQUIRE(check4d(4, 7, 2, 5) == 4725);


    REQUIRE(check4d[{4, 7, 2, 6}] == 4726);
}

TEST_CASE("Naming", "[ModelArray]")
{
    std::string dataName = "u10m";

    ModelArray named = ModelArray::HField(dataName);

    REQUIRE(named.name() == dataName);
}

TEST_CASE("Moving data", "[ModelArray]")
{
    size_t n = 10;
    ModelArray::setDimensions(ModelArray::Type::H, {n, n});

    ModelArray src = ModelArray::HField("data");
    for (int i = 0; i < n * n; ++i) {
        src[i] = i;
    }

    ModelArray cpyCtor(src);
    REQUIRE(cpyCtor.name() == src.name());
    REQUIRE(cpyCtor(2, 3) == src(2, 3));

    ModelArray cpyAss = ModelArray::HField("cpyAss");
    cpyAss = src;
    REQUIRE(cpyAss(2, 3) == 23);
}

TEST_CASE("Instance setDimensions sets instance dimensions", "[ModelArray]")
{
    ModelArray uu = ModelArray::UField("uu");
    REQUIRE(uu.size() == 0);
    ModelArray::Dimensions udim = {5, 5};
    uu.setDimensions(udim);
    REQUIRE(uu.size() == udim[0] * udim[1]);
    REQUIRE(uu.nDimensions() == 2);
    REQUIRE(uu.dimensions() == udim);
}

TEST_CASE("Intrinsic pointer functionality", "[ModelArray]")
{
    ModelArray src = ModelArray::HField("src");
    ModelArray ptr = ModelArray::UField("");
    ptr.pointAt(src);

    // Array dimensions should now match
    REQUIRE(src.dimensions() == ptr.dimensions());

    ModelArray::setDimensions(ModelArray::Type::H, {15, 15});

    size_t i = 8;
    size_t j = 7;
    double v87 = 15;
    src[{i, j}] = v87;
    REQUIRE(ptr[{i, j}] == v87);

    i = 6;
    j = 9;
    double v69 = 234;
    ptr[{i, j}] = v69;
    REQUIRE(src[{i, j}] == v69);
    v69 = 87.5;
    src[{i, j}] = v69;
    REQUIRE(ptr[{i, j}] == v69);

    // Copy constructor
    ModelArray cpy(ptr);
    REQUIRE(cpy[{i, j}] == v69);
    v69 = 4667.23;
    cpy[{i, j}] = v69;
    // Should be a deep copy, leaving the original and its reference unchanged
    REQUIRE(ptr[{i, j}] != v69);
    REQUIRE(src[{i, j}] != v69);

}
    } /* namespace Nextsim */
