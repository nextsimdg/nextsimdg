/*!
 * @file SMUIceAlbedo.hpp
 *
 * @date Sep 28, 2021
 * @author Tim Spain <timothy.spain@nersc.no>
 */

#ifndef SRC_INCLUDE_SMUICEALBEDO_HPP
#define SRC_INCLUDE_SMUICEALBEDO_HPP

#include "IIceAlbedo.hpp"

namespace Nextsim {

//! The implementation class for the SMU calculation of ice surface albedo
// with constant snow albedo.
class SMUIceAlbedo : public IIceAlbedo {
    /*!
     * @brief Calculates the SMU ice surface short wave albedo with constant
     * snow albedo.
     *
     * @param temperature The temperature of the ice surface.
     * @param snowThickness The true snow thickness on top of the ice.
     */
    double albedo(double temperature, double snowThickness);
};

}

#endif /* SRC_INCLUDE_SMUICEALBEDO_HPP */
