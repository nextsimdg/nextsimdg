/*!
 * @file FiniteElementFluxes.cpp
 *
 * @date Apr 29, 2022
 * @author Tim Spain <timothy.spain@nersc.no>
 */

#include "include/FiniteElementFluxes.hpp"

#include "include/FiniteElementSpecHum.hpp"
#include "include/IIceAlbedoModule.hpp"
#include "include/IceOceanHeatFluxModule.hpp"
#include "include/constants.hpp"

#include <memory>

namespace Nextsim {

static double stefanBoltzmannLaw(double temperature);

double FiniteElementFluxes::dragOcean_q;
double FiniteElementFluxes::dragOcean_t;
double FiniteElementFluxes::dragIce_t;
double FiniteElementFluxes::m_oceanAlbedo;
double FiniteElementFluxes::m_I0;

template <>
const std::map<int, std::string> Configured<FiniteElementFluxes>::keyMap = {
    { FiniteElementFluxes::DRAGOCEANQ_KEY, "nextsim_thermo.drag_ocean_q" },
    { FiniteElementFluxes::DRAGOCEANT_KEY, "nextsim_thermo.drag_ocean_t" },
    { FiniteElementFluxes::DRAGICET_KEY, "nextsim_thermo.drag_ice_t" },
    { FiniteElementFluxes::OCEANALBEDO_KEY, "nextsim_thermo.albedoW" },
    { FiniteElementFluxes::I0_KEY, "nextsim_thermo.I_0" },
};

void FiniteElementFluxes::configure()
{
    iIceAlbedoImpl = &Module::getImplementation<IIceAlbedo>();
    tryConfigure(iIceAlbedoImpl);

    dragOcean_q = Configured::getConfiguration(keyMap.at(DRAGOCEANQ_KEY), 1.5e-3);
    dragOcean_t = Configured::getConfiguration(keyMap.at(DRAGOCEANT_KEY), 0.83e-3);
    dragIce_t = Configured::getConfiguration(keyMap.at(DRAGICET_KEY), 1.3e-3);
    m_oceanAlbedo = Configured::getConfiguration(keyMap.at(OCEANALBEDO_KEY), 0.07);
    m_I0 = Configured::getConfiguration(keyMap.at(I0_KEY), 0.17);
}

void FiniteElementFluxes::setData(const ModelState& ms)
{
    IIceFluxes::setData(ms);
    IOWFluxes::setData(ms);

    // Data arrays can now be set to the correct size
    evap.resize();
    Q_lh_ow.resize();
    Q_sh_ow.resize();
    Q_sw_ow.resize();
    Q_lw_ow.resize();
    Q_lh_ia.resize();
    Q_sh_ia.resize();
    Q_sw_ia.resize();
    Q_lw_ia.resize();
    rho_air.resize();
    cp_air.resize();
    sh_air.resize();
    sh_water.resize();
    sh_ice.resize();
    dshice_dT.resize();
}

ModelState FiniteElementFluxes::getState() const { return ModelState(); }
ModelState FiniteElementFluxes::getState(const OutputLevel&) const { return getState(); }

void FiniteElementFluxes::calculateOW(size_t i, const TimestepTime& tst)
{
    // Mass flux from open water (evaporation)
    evap[i] = dragOcean_q * rho_air[i] * v_air[i] * (sh_water[i] - sh_air[i]);

    // Momentum flux from open water (drag pressure)
    // TODO

    // Heat flux open water
    //   Latent heat from evaporation (and condensation)
    Q_lh_ow[i] = evap[i] * latentHeatWater(sst[i]);
    //   Sensible heat
    Q_sh_ow[i] = dragOcean_t * rho_air[i] * cp_air[i] * v_air[i] * (sst[i] - t_air[i]);
    //   Shortwave flux
    Q_sw_ow[i] = -sw_in[i] * (1 - m_oceanAlbedo);
    // Longwave flux
    Q_lw_ow[i] = stefanBoltzmannLaw(sst[i]) - lw_in[i];
    // Total open water flux
    qow[i] = Q_lh_ow[i] + Q_sh_ow[i] + Q_sw_ow[i] + Q_lw_ow[i];
}

void FiniteElementFluxes::calculateIce(size_t i, const TimestepTime& tst)
{
    // Mass flux ice
    subl[i] = dragIce_t * rho_air[i] * v_air[i] * (sh_ice[i] - sh_air[i]);

    // Momentum flux is dealt with by the ice dynamics

    // Heat flux ice-atmosphere
    // Latent heat from sublimation
    Q_lh_ia[i] = subl[i] * latentHeatIce(tice.zIndexAndLayer(i, 0));
    double dmdot_dT = dragIce_t * rho_air[i] * v_air[i] * dshice_dT[i];
    double dQlh_dT = latentHeatIce(tice.zIndexAndLayer(i, 0)) * dmdot_dT;
    // Sensible heat flux
    Q_sh_ia[i]
        = dragIce_t * rho_air[i] * cp_air[i] * v_air[i] * (tice.zIndexAndLayer(i, 0) - t_air[i]);
    double dQsh_dT = dragIce_t * rho_air[i] * cp_air[i] * v_air[i];
    // Shortwave flux
    double albedoValue = iIceAlbedoImpl->albedo(tice.zIndexAndLayer(i, 0), h_snow_true[i]);
    Q_sw_ia[i] = -sw_in[i] * (1. - m_I0) * (1 - albedoValue);
    // Longwave flux
    Q_lw_ia[i] = stefanBoltzmannLaw(tice.zIndexAndLayer(i, 0)) - lw_in[i];
    double dQlw_dT
        = 4 / kelvin(tice.zIndexAndLayer(i, 0)) * stefanBoltzmannLaw(tice.zIndexAndLayer(i, 0));
    // Total flux
    qia[i] = Q_lh_ia[i] + Q_sh_ia[i] + Q_sw_ia[i] + Q_lw_ia[i];
    // Total temperature dependence of flux
    dqia_dt[i] = dQlh_dT + dQsh_dT + dQlw_dT;
}

void FiniteElementFluxes::updateAtmosphere(const TimestepTime& tst)
{
    overElements(std::bind(&FiniteElementFluxes::calculateAtmos, this, std::placeholders::_1,
                     std::placeholders::_2),
        tst);
}

void FiniteElementFluxes::calculateAtmos(size_t i, const TimestepTime& tst)
{
    // Specific humidity of...
    // ...the air
    sh_air[i] = FiniteElementSpecHum::water()(t_dew2[i], p_air[i]);
    // ...over the open ocean
    sh_water[i] = FiniteElementSpecHum::water()(sst[i], p_air[i], sss[i]);
    // ...over the ice
    std::pair<double, double> iceData
        = FiniteElementSpecHum::ice().valueAndDerivative(tice.zIndexAndLayer(i, 0), p_air[i]);
    sh_ice[i] = iceData.first;
    dshice_dT[i] = iceData.second;
    // Density of the wet air
    double Ra_wet = Air::Ra / (1 - sh_air[i] * (1 - Vapour::Ra / Air::Ra));
    rho_air[i] = p_air[i] / (Ra_wet * kelvin(t_air[i]));
    // Heat capacity of the wet air
    cp_air[i] = Air::cp + sh_air[i] * Vapour::cp;
}

double FiniteElementFluxes::latentHeatWater(double temperature)
{
    // Polynomial approximation expressed using Horner's scheme
    return Water::Lv0
        + temperature * (-2.36418e3 + temperature * (1.58927 + temperature * (-6.14342e-2)));
}

double FiniteElementFluxes::latentHeatIce(double temperature)
{
    return Water::Lv0 + Water::Lf - 240. + temperature * (-290. + temperature * (-4.));
}

template <>
const std::map<int, std::string> Configured<FiniteElementFluxCalc>::keyMap = {
    { FiniteElementFluxCalc::OW_FLUX_KEY, "OceanFluxModel" },
};

void FiniteElementFluxCalc::configure()
{
    IFluxCalculation::configure();

    // FIXME: Here we assume that no other implementation of IOWFluxes is requested
    fef = new FiniteElementFluxes;
    iOWFluxesImpl = std::unique_ptr<IOWFluxes>(fef);
    iIceFluxesImpl = fef;
    // iOWFluxesImpl = std::move(Module::getInstance<IOWFluxes>());
    fef->configure();

    iceOceanHeatFluxImpl = &Module::getImplementation<IIceOceanHeatFlux>();
    tryConfigure(iceOceanHeatFluxImpl);
}

void FiniteElementFluxCalc::setData(const ModelState& ms)
{
    IFluxCalculation::setData(ms);

    iOWFluxesImpl->setData(ms);
    iIceFluxesImpl->setData(ms);
    fef->setData(ms);
    iceOceanHeatFluxImpl->setData(ms);
}

ModelState FiniteElementFluxCalc::getState() const { return ModelState(); }
ModelState FiniteElementFluxCalc::getState(const OutputLevel&) const { return getState(); }

void FiniteElementFluxCalc::update(const TimestepTime& tst)
{
    aoState.update(tst);
    // Update the atmospheric state
    fef->updateAtmosphere(tst);
    // Call the modular open water flux calculation
    iOWFluxesImpl->updateOW(tst);
    // Call the fixed ice flux calculation
    iIceFluxesImpl->updateIce(tst);
    iceOceanHeatFluxImpl->update(tst);
}

double stefanBoltzmannLaw(double temperatureC)
{
    return Ice::epsilon * PhysicalConstants::sigma * std::pow(kelvin(temperatureC), 4);
}
} /* namespace Nextsim */
