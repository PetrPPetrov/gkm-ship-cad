// Copyright 2020 Petr Petrovich Petrov. All rights reserved.
// License: https://github.com/PetrPPetrov/gkm-ship-cad/blob/master/LICENSE

#pragma once

#include <memory>
#include "Eigen/Eigen"

namespace Gkm
{
    namespace Solid
    {
        struct NearestPointInfo
        {
            Eigen::Vector3d point;
            Eigen::Vector3d normal;
        };

        struct ISolid
        {
            typedef std::shared_ptr<ISolid> Ptr;

            virtual bool inside(const Eigen::Vector3d& point) const = 0;
            virtual Eigen::AlignedBox3d bbox() const = 0;
            //virtual NearestPointInfo calcNearestPointOnBoundary(const Eigen::Vector3d& point) const = 0;
        };

        struct Cube : public ISolid
        {
            typedef std::shared_ptr<Cube> Ptr;

            double half_edge_size = 1.0;

            virtual bool inside(const Eigen::Vector3d& point) const override;
            virtual Eigen::AlignedBox3d bbox() const override;
            //virtual NearestPointInfo calcNearestPointOnBoundary(const Eigen::Vector3d& point) const override;
        };

        struct Sphere : public ISolid
        {
            typedef std::shared_ptr<Sphere> Ptr;

            double radius = 1.0;

            virtual bool inside(const Eigen::Vector3d& point) const override;
            virtual Eigen::AlignedBox3d bbox() const override;
            //virtual NearestPointInfo calcNearestPointOnBoundary(const Eigen::Vector3d& point) const override;
        };

        struct IBooleanOperator : public ISolid
        {
            typedef std::shared_ptr<IBooleanOperator> Ptr;

            ISolid::Ptr left;
            ISolid::Ptr right;
        };

        struct UnionOperator : public IBooleanOperator
        {
            typedef std::shared_ptr<UnionOperator> Ptr;

            virtual bool inside(const Eigen::Vector3d& point) const override;
            virtual Eigen::AlignedBox3d bbox() const override;
            //virtual NearestPointInfo calcNearestPointOnBoundary(const Eigen::Vector3d& point) const override;
        };

        struct DifferenceOperator : public IBooleanOperator
        {
            typedef std::shared_ptr<DifferenceOperator> Ptr;

            virtual bool inside(const Eigen::Vector3d& point) const override;
            virtual Eigen::AlignedBox3d bbox() const override;
            //virtual NearestPointInfo calcNearestPointOnBoundary(const Eigen::Vector3d& point) const override;
        };

        struct IntersectionOperator : public IBooleanOperator
        {
            typedef std::shared_ptr<IntersectionOperator> Ptr;

            virtual bool inside(const Eigen::Vector3d& point) const override;
            virtual Eigen::AlignedBox3d bbox() const override;
            //virtual NearestPointInfo calcNearestPointOnBoundary(const Eigen::Vector3d& point) const override;
        };

        struct TransformOperator : public ISolid
        {
            typedef std::shared_ptr<TransformOperator> Ptr;

            Eigen::Vector3d translate;
            ISolid::Ptr solid;

            virtual bool inside(const Eigen::Vector3d& point) const override;
            virtual Eigen::AlignedBox3d bbox() const override;
            //virtual NearestPointInfo calcNearestPointOnBoundary(const Eigen::Vector3d& point) const override;
        };
    }
}
