/*!
 * @file DevGrid_example.cpp
 *
 * @date Jan 7, 2022
 * @author Tim Spain <timothy.spain@nersc.no>
 */

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "include/DevGrid.hpp"
#include "include/DevGridIO.hpp"
#include "include/ElementData.hpp"
#include "include/IStructure.hpp"
#include "include/ModuleLoader.hpp"

#include <fstream>

const std::string filename = "example.nc";

namespace Nextsim {

TEST_CASE("Write out a DevGrid restart file", "[DevGrid]")
{
    ModuleLoader::getLoader().setAllDefaults();

    DevGrid grid;
    grid.init("");
    grid.setIO(new DevGridIO(grid));
    // Fill in the data. It is not real data.
    grid.resetCursor();
    int nx = DevGrid::nx;
    int ny = DevGrid::nx;
    double yFactor = 0.01;
    double xFactor = 0.0001;

    for (int j = 0; j < ny; ++j) {
        for (int i = 0; i < nx; ++i) {
            if (grid.validCursor()) {
                double fractional = j * yFactor + i * xFactor;
                grid.cursorData() = PrognosticGenerator().hice(1 + fractional).cice(2 + fractional).sst(3 + fractional).sss(4 + fractional).hsnow(5 + fractional).tice({ -(1. + fractional) });
                grid.incrCursor();
            }
        }
    }

    grid.dump(filename);
    WARN("File written to " << filename);
}

TEST_CASE("Read the example file, if it exists", "[DevGrid]")
{
    ModuleLoader::getLoader().setAllDefaults();

    DevGrid grid;
    grid.init("");
    grid.resetCursor();
    int targetIndex = 7 * DevGrid::nx + 3;
    for (int i = 0; i < targetIndex; ++i) {
        grid.incrCursor();
    }
    if (!grid.validCursor()) {
        FAIL("Invalid cursor value of " << targetIndex);
    }

    double unInitIce = grid.cursorData().iceThickness();

    grid.setIO(new DevGridIO(grid));
    grid.init(filename);

    grid.resetCursor();
    for (int i = 0; i < targetIndex; ++i) {
        grid.incrCursor();
    }
    if (!grid.validCursor()) {
        FAIL("Invalid cursor value of " << targetIndex);
    }

    REQUIRE(grid.cursorData().iceThickness() != 0);
    REQUIRE(grid.cursorData().iceThickness() > 1);
    REQUIRE(grid.cursorData().iceThickness() < 2);
    REQUIRE(grid.cursorData().iceThickness() == 1.0703);
    REQUIRE(grid.cursorData().iceThickness() != unInitIce);
}
}
