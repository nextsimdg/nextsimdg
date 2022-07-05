/*!
 * @file OceanState.hpp
 *
 * @date May 9, 2022
 * @author Tim Spain <timothy.spain@nersc.no>
 */

#ifndef OCEANSTATE_HPP
#define OCEANSTATE_HPP

#include "include/Configured.hpp"
#include "include/IFreezingPointModule.hpp"
#include "include/ModelComponent.hpp"

namespace Nextsim {

//! A class providing an interface to the ocean climatologies or coupled model.
class OceanState : public ModelComponent, public Configured<OceanState> {
public:
    OceanState();
    virtual ~OceanState() = default;

    void setData(const ModelState&) override;
    ModelState getState() const override;
    ModelState getState(const OutputLevel&) const override;
    std::string getName() const override;
    std::unordered_set<std::string> hFields() const override;

    void configure() override;
    /*!
     * @brief Updates the ocean state.
     *
     * @details Performs any common calculations, then any implementation
     * specific updates.
     *
     * @param tStep The object containing the timestep start and duration times.
     */
    void update(const TimestepTime&);

protected:
    HField sst;
    HField sss;
    HField mld;

    HField tf;
    HField cpml;

    IFreezingPoint* tfImpl;

    virtual void updateSpecial(const TimestepTime&) = 0;

private:
    void updateFreezingPoint(size_t i, const TimestepTime&);
};

} /* namespace Nextsim */

#endif /* OCEANSTATE_HPP */
