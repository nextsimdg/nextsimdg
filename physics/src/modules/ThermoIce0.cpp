/*!
 * @file ThermoIce0.cpp
 *
 * @date Mar 17, 2022
 * @author Tim Spain <timothy.spain@nersc.no>
 */

#include "include/ThermoIce0.hpp"

#include "include/IFreezingPointModule.hpp"
#include "include/IceGrowth.hpp"
#include "include/ModelArray.hpp"
#include "include/constants.hpp"

namespace Nextsim {

double ThermoIce0::k_s;
const double ThermoIce0::freezingPointIce = -Water::mu * Ice::s;

ThermoIce0::ThermoIce0()
    : IIceThermodynamics()
    , snowMelt(ModelArray::Type::H)
    , topMelt(ModelArray::Type::H)
    , botMelt(ModelArray::Type::H)
    , qic(ModelArray::Type::H)
{
}

void ThermoIce0::update(const TimestepTime& tsTime)
{
    overElements(std::bind(&ThermoIce0::calculateElement, this, std::placeholders::_1,
                     std::placeholders::_2),
        tsTime);
}

template <>
const std::map<int, std::string> Configured<ThermoIce0>::keyMap = {
    { ThermoIce0::KS_KEY, "thermoice0.ks" },
};

void ThermoIce0::configure() { k_s = Configured::getConfiguration(keyMap.at(KS_KEY), 0.3096); }

void ThermoIce0::setData(const ModelState& ms)
{
    IIceThermodynamics::setData(ms);

    snowMelt.resize();
    topMelt.resize();
    botMelt.resize();
    qic.resize();
}

void ThermoIce0::calculateElement(size_t i, const TimestepTime& tst)
{
    static const double bulkLHFusionSnow = Water::Lf * Ice::rhoSnow;
    static const double bulkLHFusionIce = Water::Lf * Ice::rho;

    double& tice_i = tice.zIndexAndLayer(i, 0);
    double k_lSlab = k_s * Ice::kappa / (k_s * hice[i] + Ice::kappa * hsnow[i]);
    qic[i] = k_lSlab * (tf[i] - tice0.zIndexAndLayer(i, 0));
    double remainingFlux = qic[i] - qia[i];
    tice_i = tice0.zIndexAndLayer(i, 0) + remainingFlux / (k_lSlab + dQia_dt[i]);

    // Clamp the temperature of the ice to a maximum of the melting point
    // of ice or snow
    double meltingLimit = (hsnow[i] > 0) ? 0 : freezingPointIce;
    tice_i = std::min(meltingLimit, tice_i);

    // Top melt. Melting rate is non-positive.
    double snowMeltRate = std::min(-remainingFlux, 0.) / bulkLHFusionSnow;
    snowMelt[i] = snowMeltRate * tst.step;
    double snowSublRate = sublim[i] / Ice::rhoSnow;
    double nowSnow = hsnow[i] + (snowMeltRate - snowSublRate) * tst.step;
    // Use excess flux to melt ice. Non-positive value
    double excessIceMelt = std::min(nowSnow, 0.) * bulkLHFusionSnow / bulkLHFusionIce;
    // With the excess flux noted, clamp the snow thickness to a minimum of zero.
    hsnow[i] = std::max(nowSnow, 0.);
    // Then add snowfall back on top
    hsnow[i] += snowfall[i] * tst.step / Ice::rhoSnow;

    // Bottom melt or growth
    double iceBottomChange = (qic[i] - qio[i]) * tst.step / bulkLHFusionIce;
    // Total thickness change
    deltaHi[i] = excessIceMelt + iceBottomChange;
    hice[i] += deltaHi[i];

    // Amount of melting (only) at the top and bottom of the ice
    topMelt[i] = std::min(excessIceMelt, 0.);
    botMelt[i] = std::min(iceBottomChange, 0.);
    // Snow to ice conversion
    double iceDraught = (hice[i] * Ice::rho + hsnow[i] * Ice::rhoSnow) / Water::rhoOcean;

    if (doFlooding && iceDraught > hice[i]) {
        double snowDraught = iceDraught - hice[i];
        snowToIce[i] = snowDraught;
        hsnow[i] -= snowDraught * Ice::rho / Ice::rhoSnow;
        hice[i] = iceDraught;
    } else {
        snowToIce[i] = 0;
    }

    // Melt all ice if it is below minimum threshold
    if (hice[i] < IceGrowth::minimumIceThickness()) {
        if (deltaHi[i] < 0) {
            double scaling = oldHi[i] / deltaHi[i];
            topMelt[i] *= scaling;
            botMelt[i] *= scaling;
        }

        // No snow was converted to ice
        snowToIce[i] = 0.;

        // Change in thickness is all of the old thickness
        deltaHi[i] = -oldHi[i];

        // The ice-ocean flux includes all the latent heat
        qio[i] += hice[i] * bulkLHFusionIce / tst.step + hsnow[i] * bulkLHFusionSnow / tst.step;

        // No ice, no snow and the surface temperature is the melting point of ice
        hice[i] = 0.;
        hsnow[i] = 0.;
        tice.zIndexAndLayer(i, 0) = Module::getImplementation<IFreezingPoint>()(sss[i]);
    }
}
} /* namespace Nextsim */
