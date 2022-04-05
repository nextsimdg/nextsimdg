/*!
 * @file IVerticalIceGrowth.hpp
 *
 * @date Mar 16, 2022
 * @author Tim Spain <timothy.spain@nersc.no>
 */

#ifndef IVERTICALICEGROWTH_HPP
#define IVERTICALICEGROWTH_HPP

#include "include/ModelComponent.hpp"
#include "include/ModelArray.hpp"
#include "include/Time.hpp"

namespace Nextsim {
class IVerticalIceGrowth : public ModelComponent {
public:
    ~IVerticalIceGrowth() = default;

    std::string getName() const override { return "VerticalIceGrowth"; }
    void setData(const ModelState& ms) override { }
    ModelState getState() const override { return ModelState(); }
    ModelState getState(const OutputLevel&) const override { return getState(); }
    virtual void update(const TimestepTime& tsTime) = 0;

protected:
    IVerticalIceGrowth()
    {
        registerModule();
        ModelComponent::requestSharedArray(SharedArray::H_ICE, &hice);
        ModelComponent::requestSharedArray(SharedArray::C_ICE, &cice);
        ModelComponent::requestSharedArray(SharedArray::H_SNOW, &hsnow);
        ModelComponent::requestSharedArray(SharedArray::T_ICE, &tice);
        ModelComponent::requestSharedArray(SharedArray::Q_IC, &qic);
        ModelComponent::requestSharedArray(SharedArray::Q_IO, &qio);
        ModelComponent::requestProtectedArray(SharedArray::Q_IA, &qia);
        ModelComponent::requestProtectedArray(SharedArray::DQIA_DT, &dQia_dT);
        ModelComponent::requestProtectedArray(SharedArray::SUBLIM, &sublim);
        ModelComponent::requestProtectedArray(ProtectedArray::SNOW, &snowfall);
        ModelComponent::requestProtectedArray(ProtectedArray::SSS, &sss);
    }

    HField* hice; // From IceGrowth
    HField* cice; // From IceGrowth
    HField* hsnow; // From Ice Growth?
    ZField* tice; // Updated value from IceTemperature
    HField* qic; // From IceTemperature. Conductive heat flux to the ice surface.
    HField* qio; // From FluxCalculation
    const HField* qia; // From FluxCalculation
    const HField* dQia_dT; // From FluxCalculation
    const HField* sublim; // From AtmosphereState
    const HField* snowfall; // From ExternalData
    const HField* sss; // From ExternalData (possibly PrognosticData)
    // Owned, Module-private arrays
    HField snowToIce;
};

} /* namespace Nextsim */

#endif /* IVERTICALICEGROWTH_HPP */