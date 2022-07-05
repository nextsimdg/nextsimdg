/*!
 * @file FluxCalculationModule.cpp
 *
 * @date Apr 29, 2022
 * @author Tim Spain <timothy.spain@nersc.no>
 */

#include "include/FluxCalculationModule.hpp"

#include "include/FiniteElementFluxes.hpp"

#include <string>

namespace Module {
const std::string FINITEELEMENTFLUXES = "Nextsim::FiniteElementFluxCalc";

template <>
Module<Nextsim::IFluxCalculation>::map Module<Nextsim::IFluxCalculation>::functionMap = {
    { FINITEELEMENTFLUXES, newImpl<Nextsim::IFluxCalculation, Nextsim::FiniteElementFluxCalc> },
};

template <>
Module<Nextsim::IFluxCalculation>::fn Module<Nextsim::IFluxCalculation>::spf
    = functionMap.at(FINITEELEMENTFLUXES);
template <>
std::unique_ptr<Nextsim::IFluxCalculation> Module<Nextsim::IFluxCalculation>::staticInstance
    = std::move(newImpl<Nextsim::IFluxCalculation, Nextsim::FiniteElementFluxCalc>());

template <> std::string Module<Nextsim::IFluxCalculation>::moduleName()
{
    return "Nextsim::IFluxCalculation";
}

template <> Nextsim::IFluxCalculation& getImplementation<Nextsim::IFluxCalculation>()
{
    return getImplTemplate<Nextsim::IFluxCalculation, FluxCalculationModule>();
}
template <> void setImplementation<Nextsim::IFluxCalculation>(const std::string& implName)
{
    setImplTemplate<FluxCalculationModule>(implName);
}
template <> std::unique_ptr<Nextsim::IFluxCalculation> getInstance()
{
    return getInstTemplate<Nextsim::IFluxCalculation, FluxCalculationModule>();
}
FluxCalculationModule::Constructor FluxCalculationModule::ctor;
FluxCalculationModule::Constructor::Constructor()
{
    addToConfiguredModules<Nextsim::IFluxCalculation, FluxCalculationModule>();
}

} /* namespace Module */
