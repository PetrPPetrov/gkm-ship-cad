// Copyright 2020 Petr Petrovich Petrov. All rights reserved.
// License: https://github.com/PetrPPetrov/gkm-ship-cad/blob/master/LICENSE

#include "gkm_solid/gkm_solid.h"

bool Gkm::Solid::Cube::inside(const Eigen::Vector3d& point) const
{
    return std::fabs(point.x()) <= half_edge_size && std::fabs(point.y()) <= half_edge_size && std::fabs(point.z()) <= half_edge_size;
}

Eigen::AlignedBox3d Gkm::Solid::Cube::bbox() const
{
    Eigen::AlignedBox3d bbox;
    bbox.min() = Eigen::Vector3d(-half_edge_size, -half_edge_size, -half_edge_size);
    bbox.max() = Eigen::Vector3d(half_edge_size, half_edge_size, half_edge_size);
    return bbox;
}

//Gkm::Solid::NearestPointInfo Gkm::Solid::Cube::calcNearestPointOnBoundary(const Eigen::Vector3d& point) const
//{
//    Gkm::Solid::Cube::
//}

bool Gkm::Solid::Sphere::inside(const Eigen::Vector3d& point) const
{
    const double length = point.squaredNorm();
    return length <= radius;
}

Eigen::AlignedBox3d Gkm::Solid::Sphere::bbox() const
{
    Eigen::AlignedBox3d bbox;
    bbox.min() = Eigen::Vector3d(-radius, -radius, -radius);
    bbox.max() = Eigen::Vector3d(radius, radius, radius);
    return bbox;
}

bool Gkm::Solid::UnionOperator::inside(const Eigen::Vector3d& point) const
{
    return left->inside(point) || right->inside(point);
}

Eigen::AlignedBox3d Gkm::Solid::UnionOperator::bbox() const
{
    Eigen::AlignedBox3d bbox = left->bbox();
    return bbox.merged(right->bbox());
}

bool Gkm::Solid::DifferenceOperator::inside(const Eigen::Vector3d& point) const
{
    return left->inside(point) && !right->inside(point);
}

Eigen::AlignedBox3d Gkm::Solid::DifferenceOperator::bbox() const
{
    return left->bbox();
}

bool Gkm::Solid::IntersectionOperator::inside(const Eigen::Vector3d& point) const
{
    return left->inside(point) && right->inside(point);
}

Eigen::AlignedBox3d Gkm::Solid::IntersectionOperator::bbox() const
{
    return left->bbox();
}

bool Gkm::Solid::TransformOperator::inside(const Eigen::Vector3d& point) const
{
    return solid->inside(point - translate);
}

Eigen::AlignedBox3d Gkm::Solid::TransformOperator::bbox() const
{
    Eigen::AlignedBox3d bbox = solid->bbox();
    bbox.min() += translate;
    bbox.max() += translate;
    return bbox;
}
