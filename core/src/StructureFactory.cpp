/*!
 * @file StructureFactory.cpp
 *
 * @date Jan 18, 2022
 * @author Tim Spain <timothy.spain@nersc.no>
 */

#include "include/StructureFactory.hpp"

#include "include/IStructureModule.hpp"

#include "include/DevGrid.hpp"
#include "include/DevGridIO.hpp"

#include "include/RectGridIO.hpp"
#include "include/RectangularGrid.hpp"

#include <ncFile.h>
#include <ncGroup.h>
#include <ncGroupAtt.h>
#include <stdexcept>
#include <string>

namespace Nextsim {

std::string structureNameFromFile(const std::string& filePath)
{
    netCDF::NcFile ncf(filePath, netCDF::NcFile::read);
    netCDF::NcGroup metaGroup(ncf.getGroup(IStructure::metadataNodeName()));
    netCDF::NcGroupAtt att = metaGroup.getAtt(IStructure::typeNodeName());
    int len = att.getAttLength();
    // Initialize a std::string of len, filled with zeros
    std::string structureName(len, '\0');
    // &str[0] gives access to the buffer, guaranteed by C++11
    att.getValues(&structureName[0]);
    ncf.close();

    return structureName;
}

ModelState StructureFactory::stateFromFile(const std::string& filePath)
{
    std::string structureName = structureNameFromFile(filePath);
    // TODO There must be a better way
    if (DevGrid::structureName == structureName) {
        DevGrid gridIn;
        gridIn.setIO(new DevGridIO(gridIn));
        return gridIn.getModelState(filePath);
    } else if (RectangularGrid::structureName == structureName) {
        RectangularGrid gridIn;
        gridIn.setIO(new RectGridIO(gridIn));
        // return gridIn.getModelState(filePath);
        return ModelState();
    } else {
        throw std::invalid_argument(
            std::string("fileFromName: structure not implemented: ") + structureName);
    }
    throw std::invalid_argument(std::string("fileFromName: structure not implemented: ")
        + structureName + "\nAlso, how did you get here?");
    return ModelState();
}

void StructureFactory::fileFromState(const ModelState& state, const std::string& filePath)
{
    std::string structureName = Module::getImplementation<IStructure>().structureType();

    if (DevGrid::structureName == structureName) {
        DevGrid gridOut;
        gridOut.setIO(new DevGridIO(gridOut));
        gridOut.dumpModelState(state, filePath);
    } else if (RectangularGrid::structureName == structureName) {
        RectangularGrid gridOut;
        gridOut.setIO(new RectGridIO(gridOut));
        gridOut.dumpModelState(state, filePath);
    } else {
        throw std::invalid_argument(
            std::string("fileFromName: structure not implemented: ") + structureName);
    }
}

} /* namespace Nextsim */
