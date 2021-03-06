/*!
 * @file BasicIceOceanHeatFlux.hpp
 *
 * @date Oct 19, 2021
 * @author Tim Spain <timothy.spain@nersc.no>
 */

#ifndef SRC_INCLUDE_BASICICEOCEANHEATFLUX_HPP_
#define SRC_INCLUDE_BASICICEOCEANHEATFLUX_HPP_

#include "IIceOceanHeatFlux.hpp"

namespace Nextsim {

//! The implementation class for the basic ice-ocean heat flux.
class BasicIceOceanHeatFlux : public IIceOceanHeatFlux {
public:
    BasicIceOceanHeatFlux() = default;
    virtual ~BasicIceOceanHeatFlux() = default;

    /*!
     * @brief Calculate the basic ice-ocean heat flux.
     *
     * @param prog PrognosticData for this element (constant).
     * @param exter ExternalData for this element (constant).
     * @param phys PhysicsData for this element (constant).
     * @param nsphys Nextsim physics implementation data for this element
     * (constant).
     */
    double flux(const PrognosticData&, const ExternalData&, const PhysicsData&, const NextsimPhysics&) override;
};

} /* namespace Nextsim */

#endif /* SRC_INCLUDE_BASICICEOCEANHEATFLUX_HPP_ */
