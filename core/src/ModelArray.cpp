/*!
 * @file ModelData.cpp
 *
 * @date Feb 24, 2022
 * @author Tim Spain <timothy.spain@nersc.no>
 */

#include "include/ModelArray.hpp"

#include <algorithm>
#include <cstdarg>
#include <iterator>
#include <set>
#include <string>
#include <utility>

namespace Nextsim {

ModelArray::SizeMap ModelArray::m_sz;
ModelArray::DimensionMap ModelArray::m_dims;

const std::map<ModelArray::Type, std::string> ModelArray::typeNames = {
    { ModelArray::Type::H, "HField" },
    { ModelArray::Type::U, "UField" },
    { ModelArray::Type::V, "VField" },
    { ModelArray::Type::Z, "ZField" },
    { ModelArray::Type::DG, "DGHField--DO-NOT-USE--" },
};

ModelArray::ModelArray(const Type type)
    : type(type)
{
    m_data.resize(std::max(std::size_t { 0 }, m_sz.at(type)), nComponents());
}

ModelArray::ModelArray()
    : ModelArray(Type::H)
{
}

ModelArray::ModelArray(const ModelArray& orig)
    : ModelArray(orig.type)
{
    setData(orig.m_data);
}

ModelArray& ModelArray::operator=(const ModelArray& orig)
{
    type = orig.type;
    setData(orig.m_data);

    return *this;
}

ModelArray& ModelArray::operator=(const double& fill)
{
    setData(fill);

    return *this;
}

ModelArray ModelArray::operator+(const ModelArray& addend) const
{
    ModelArray result = *this;
    return result += addend;
}

ModelArray ModelArray::operator-(const ModelArray& subtrahend) const
{
    ModelArray result = *this;
    return result -= subtrahend;
}

ModelArray ModelArray::operator*(const ModelArray& multiplier) const
{
    ModelArray result = *this;
    return result *= multiplier;
}

ModelArray ModelArray::operator/(const ModelArray& divisor) const
{
    ModelArray result = *this;
    return result /= divisor;
}

ModelArray ModelArray::operator-() const
{
    ModelArray copy(type);
    copy.m_data = -m_data;
    return copy;
}

ModelArray ModelArray::operator+(const double& x) const
{
    ModelArray result = *this;
    return result += x;
}

ModelArray ModelArray::operator-(const double& x) const
{
    ModelArray result = *this;
    return result -= x;
}

ModelArray ModelArray::operator*(const double& x) const
{
    ModelArray result = *this;
    return result *= x;
}

ModelArray ModelArray::operator/(const double& x) const
{
    ModelArray result = *this;
    return result /= x;
}

ModelArray operator+(const double& x, const ModelArray& y) { return y + x; }

ModelArray operator-(const double& x, const ModelArray& y) { return -(y - x); }

ModelArray operator*(const double& x, const ModelArray& y) { return y * x; }

ModelArray operator/(const double& x, const ModelArray& y)
{
    ModelArray xArray(y.getType());
    xArray.setData(x);
    return xArray /= y;
}

void ModelArray::setData(double value)
{
    resize();
    m_data = value;
}

void ModelArray::setData(const double* pData)
{
    resize();
    auto out = std::copy(pData, pData + m_sz.at(type), m_data.data());
}

void ModelArray::setData(const DataType& from) { setData(from.data()); }

void ModelArray::setData(const ModelArray& from) { setData(from.m_data.data()); }

void ModelArray::setDimensions(Type type, const Dimensions& newDims)
{
    size_t newSize = 1;
    for (size_t dimLen : newDims) {
        newSize *= dimLen;
    }
    m_dims.at(type).clear();
    std::copy(newDims.begin(), newDims.end(), std::back_inserter(m_dims.at(type)));
    m_sz.at(type) = newSize;
}

const double& ModelArray::operator[](const Dimensions& loc) const
{
    return (*this)[indexr(this->dimensions().data(), loc)];
}

double& ModelArray::operator[](const Dimensions& dims)
{
    return const_cast<double&>(std::as_const(*this)[dims]);
}

ModelArray::Component ModelArray::components(const Dimensions& loc)
{
    return components(indexr(dimensions().data(), loc));
}
} /* namespace Nextsim */
