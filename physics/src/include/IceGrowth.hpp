/*!
 * @file IceGrowth.hpp
 *
 * @date Mar 15, 2022
 * @author Tim Spain <timothy.spain@nersc.no>
 */

#ifndef PHYSICS_SRC_INCLUDE_ICEGROWTH_HPP
#define PHYSICS_SRC_INCLUDE_ICEGROWTH_HPP

#include "include/ModelComponent.hpp"
#include "include/Configured.hpp"
#include "include/IVerticalIceGrowth.hpp"
#include "include/Time.hpp"

namespace Nextsim {

class IceGrowth : public ModelComponent, public Configured<IceGrowth> {
public:
    IceGrowth() = default;
    virtual ~IceGrowth() = default;

    void configure() override;
    enum {
        VERTICAL_GROWTH_KEY,
        LATERAL_GROWTH_KEY,
    };

    std::string getName() const override { return "IceGrowth"; }

    void setData(const ModelState&) override {};
    ModelState getState() const override { return ModelState(); }
    ModelState getState(const OutputLevel&) const override { return getState(); }

    std::set<std::string> hFields() const override { return { "updated_hice", "updated_cice" }; }
    std::set<std::string> uFields() const override { return {}; }
    std::set<std::string> vFields() const override { return {}; }
    std::set<std::string> zFields() const override { return {}; }

    void update(const TimestepTime&);

private:
    // Vertical Growth ModelComponent & Module
    std::unique_ptr<IVerticalIceGrowth> iVertical;
    // Lateral Growth ModuleComponent & Module
};

} /* namespace Nextsim */

#endif /* PHYSICS_SRC_INCLUDE_ICEGROWTH_HPP */