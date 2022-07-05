/*!
 * @file ModelComponent.cpp
 *
 * @date Feb 28, 2022
 * @author Tim Spain <timothy.spain@nersc.no>
 */

#include "include/ModelComponent.hpp"

namespace Nextsim {

std::unordered_map<std::string, ModelComponent*> ModelComponent::registeredModules;
ModelArray* ModelComponent::sharedArrays[static_cast<size_t>(SharedArray::COUNT)];
const ModelArray* ModelComponent::protectedArrays[static_cast<size_t>(ProtectedArray::COUNT)];

ModelComponent::ModelComponent() { }

void ModelComponent::setAllModuleData(const ModelState& stateIn)
{
    for (auto entry : registeredModules) {
        entry.second->setData(stateIn);
    }
}
ModelState ModelComponent::getAllModuleState()
{
    ModelState overallState;
    for (auto entry : registeredModules) {
        overallState.merge(entry.second->getState());
    }
    return overallState;
}

void ModelComponent::registerModule() { registeredModules[getName()] = this; }

void ModelComponent::unregisterAllModules() { registeredModules.clear(); }

void ModelComponent::getAllFieldNames(std::unordered_set<std::string>& uF,
    std::unordered_set<std::string>& vF, std::unordered_set<std::string>& zF)
{
    for (auto entry : registeredModules) {
        uF.merge(entry.second->uFields());
        vF.merge(entry.second->vFields());
        zF.merge(entry.second->zFields());
    }
}

void ModelComponent::registerSharedArray(SharedArray type, ModelArray* addr)
{
    // Assignment of pointer in array
    sharedArrays[static_cast<size_t>(type)] = addr;
}

void ModelComponent::registerProtectedArray(ProtectedArray type, const ModelArray* addr)
{
    // Assignment of pointer in array
    protectedArrays[static_cast<size_t>(type)] = addr;
}

} /* namespace Nextsim */
