/*
 * @file IIceOceanHeatFlux.hpp
 *
 * @date Oct 19, 2021
 * @author Tim Spain <timothy.spain@nersc.no>
 */

#ifndef IICEOCEANHEATFLUX_HPP
#define IICEOCEANHEATFLUX_HPP

#include "include/ModelArray.hpp"
#include "include/ModelArrayRef.hpp"
#include "include/ModelComponent.hpp"
#include "include/Time.hpp"

namespace Nextsim {
class PrognosticElementData;
class ExternalData;
class PhysicsData;
class NextsimPhysics;

//! The interface class for the ice-ocean heat flux calculation.
class IIceOceanHeatFlux : public ModelComponent {
public:
    virtual ~IIceOceanHeatFlux() = default;

    // This superclass has no state
    void setData(const ModelState&) override {};
    ModelState getState() const override { return ModelState(); }
    ModelState getState(const OutputLevel&) const override { return getState(); }
    // …but it does have a name
    std::string getName() const override { return "IIceOceanHeatFlux"; }

    /*!
     * Updates the ice ocean heat flux calculation for the timestep.
     *
     * @param tStep The object containing the timestep start and duration times.
     */
    virtual void update(const TimestepTime&) = 0;

protected:
    ModelArrayRef<ProtectedArray::SST> sst;
    ModelArrayRef<ProtectedArray::TF> tf;

    ModelArrayRef<SharedArray::Q_IO, RW> qio;
};
}
#endif /* SRC_INCLUDE_IICEOCEANHEATFLUX_HPP_ */
