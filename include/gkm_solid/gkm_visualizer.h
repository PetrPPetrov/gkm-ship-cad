// Copyright 2020 Petr Petrovich Petrov. All rights reserved.
// License: https://github.com/PetrPPetrov/gkm-ship-cad/blob/master/LICENSE

#pragma once

#include <memory>
#include <vector>
#include "Eigen/Eigen"
#include "gkm_solid/gkm_solid.h"

namespace Gkm
{
    namespace Solid
    {
        struct Model
        {
            typedef std::shared_ptr<Model> Ptr;

            std::vector<Eigen::Vector3f> points;
            std::vector<Eigen::Vector3i> triangles;
        };

        Model::Ptr buildModel(const ISolid::Ptr& solid);
    }
}
