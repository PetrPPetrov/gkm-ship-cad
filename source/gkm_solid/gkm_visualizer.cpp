// Copyright 2020 Petr Petrovich Petrov. All rights reserved.
// License: https://github.com/PetrPPetrov/gkm-ship-cad/blob/master/LICENSE

#include "gkm_solid/gkm_visualizer.h"

namespace
{
    struct Point;

    struct Point
    {
        Point* end_cube_x = nullptr;
        Point* end_cube_y = nullptr;
        Point* end_cube_z = nullptr;
        Point* end_cube_xy = nullptr;
        Point* end_cube_xz = nullptr;
        Point* end_cube_yz = nullptr;
        Point* end_cube_xyz = nullptr;

        Point* next_point_x = nullptr;
        Point* next_point_y = nullptr;
        Point* next_point_z = nullptr;

        Point* prev_point_x = nullptr;
        Point* prev_point_y = nullptr;
        Point* prev_point_z = nullptr;

        Eigen::Vector3d point;
        bool hollow = false;

        bool isCube() const;
        void addNextX(Point* point);
        void addPrevX(Point* point);
        void addNextY(Point* point);
        void addPrevY(Point* point);
        void addNextZ(Point* point);
        void addPrevZ(Point* point);
    };

    bool Point::isCube() const
    {
        return end_cube_x && end_cube_y && end_cube_z && end_cube_xy && end_cube_xz && end_cube_yz && end_cube_xyz;
    }

    void Point::addNextX(Point* point)
    {
        Point* next_x = next_point_x;
        next_point_x = point;
        point->prev_point_x = this;
        point->next_point_x = nullptr;
        if (next_x)
        {
            next_x->prev_point_x = point;
            point->next_point_x = next_x;
        }
    }

    void Point::addPrevX(Point* point)
    {
        Point* prev_x = prev_point_x;
        prev_point_x = point;
        point->next_point_x = this;
        point->prev_point_x = nullptr;
        if (prev_x)
        {
            prev_x->next_point_x = point;
            point->prev_point_x = prev_x;
        }
    }

    void Point::addNextY(Point* point)
    {
        Point* next_y = next_point_y;
        next_point_y = point;
        point->prev_point_y = this;
        point->next_point_y = nullptr;
        if (next_y)
        {
            next_y->prev_point_y = point;
            point->next_point_y = next_y;
        }
    }

    void Point::addPrevY(Point* point)
    {
        Point* prev_y = prev_point_y;
        prev_point_y = point;
        point->next_point_y = this;
        point->prev_point_y = nullptr;
        if (prev_y)
        {
            prev_y->next_point_y = point;
            point->prev_point_y = prev_y;
        }
    }

    void Point::addNextZ(Point* point)
    {
        Point* next_z = next_point_z;
        next_point_z = point;
        point->prev_point_z = this;
        point->next_point_z = nullptr;
        if (next_z)
        {
            next_z->prev_point_z = point;
            point->next_point_z = next_z;
        }
    }

    void Point::addPrevZ(Point* point)
    {
        Point* prev_z = prev_point_z;
        prev_point_z = point;
        point->next_point_z = this;
        point->prev_point_z = nullptr;
        if (prev_z)
        {
            prev_z->next_point_z = point;
            point->prev_point_z = prev_z;
        }
    }

    struct Cubes
    {
        std::list<std::vector<Point>> points;
        size_t last_index = 0;
        const static size_t PAGE_SIZE = 1024;

        Cubes();
        Point* AllocatePoint();
    };

    Cubes::Cubes()
    {
        std::vector<Point> new_cubes;
        new_cubes.reserve(PAGE_SIZE);
        points.push_back(std::move(new_cubes));
    }

    Point* Cubes::AllocatePoint()
    {
        if (last_index >= PAGE_SIZE)
        {
            last_index = 0;
            std::vector<Point> new_cubes;
            new_cubes.reserve(PAGE_SIZE);
            points.push_back(std::move(new_cubes));
        }
        points.back().push_back(Point());
        return &points.back()[last_index++];
    }

    class ModelBuilder
    {
        double TOLERANCE = 0.01;
        Gkm::Solid::ISolid::Ptr solid;
        Cubes cubes;

        Point* addPointX(Point* start_x, Point* end_x);
        Point* addPointY(Point* start_y, Point* end_y);
        Point* addPointZ(Point* start_z, Point* end_z);
        bool split(Point* cube);
        bool checkAndSplitCube(Point* cube);
        void checkAndSplitCubes();

    public:
        ModelBuilder(const Gkm::Solid::ISolid::Ptr& solid);
        Gkm::Solid::Model::Ptr build();
    };

    Point* ModelBuilder::addPointX(Point* start_x, Point* end_x)
    {
        const Eigen::Vector3d min = start_x->point;
        const Eigen::Vector3d max = end_x->point;
        const Eigen::Vector3d sizes = max - min;
        Eigen::Vector3d mid = min;
        mid.x() += sizes.x() / 2;
        Point* cur_point = start_x;
        while (cur_point != end_x)
        {
            if (cur_point->point.x() == mid.x())
            {
                return cur_point;
            }
            cur_point = cur_point->next_point_x;
        }
        Point* new_point = cubes.AllocatePoint();
    }

    Point* ModelBuilder::addPointY(Point* start_y, Point* end_y)
    {

    }

    Point* ModelBuilder::addPointZ(Point* start_z, Point* end_z)
    {

    }

    bool ModelBuilder::split(Point* cube)
    {
        const Eigen::Vector3d min = cube->point;
        const Eigen::Vector3d max = cube->end_cube_xyz->point;
        const Eigen::Vector3d sizes = max - min;

        if (sizes.x() < TOLERANCE && sizes.y() < TOLERANCE && sizes.z() < TOLERANCE)
        {
            return true;
        }

        const Eigen::Vector3d mid = min + sizes / 2;
        //const Eigen::Vector3d 
    }

    bool ModelBuilder::checkAndSplitCube(Point* cube)
    {
        constexpr size_t CHECK_COUNT = 5;
        static_assert(CHECK_COUNT > 0, "CHECK_COUNT must be greater than 0");

        const Eigen::Vector3d min = cube->point;
        const Eigen::Vector3d max = cube->end_cube_xyz->point;
        const Eigen::Vector3d sizes = max - min;

        const double dx = sizes.x() / (CHECK_COUNT - 1);
        const double dy = sizes.y() / (CHECK_COUNT - 1);
        const double dz = sizes.z() / (CHECK_COUNT - 1);

        bool all_outside = true;
        bool all_inside = true;

        for (size_t ix = 0; ix < CHECK_COUNT; ++ix)
        {
            for (size_t iy = 0; iy < CHECK_COUNT; ++iy)
            {
                for (size_t iz = 0; iz < CHECK_COUNT; ++iz)
                {
                    const double point_x = min.x() + dx * ix;
                    const double point_y = min.y() + dy * iy;
                    const double point_z = min.z() + dz * iz;

                    if (solid->inside(Eigen::Vector3d(point_x, point_y, point_z)))
                    {
                        all_outside = false;
                    }
                    else
                    {
                        all_inside = false;
                    }
                }
            }
        }
        if (all_inside)
        {
            // Cube is OK, pass it
            return true;
        }
        if (all_outside)
        {
            cube->hollow = true;
            return true;
        }
        return split(cube);
    }

    void ModelBuilder::checkAndSplitCubes()
    {
        for (auto point_list_it = cubes.points.begin(); point_list_it != cubes.points.end(); ++point_list_it)
        {
            auto& point_list = *point_list_it;
            for (auto point_it = point_list.begin(); point_it != point_list.end();)
            {
                auto& point = *point_it;
                if (point.isCube())
                {
                    if (checkAndSplitCube(&point))
                    {
                        ++point_it;
                    }
                }
            }
        }
    }

    ModelBuilder::ModelBuilder(const Gkm::Solid::ISolid::Ptr& solid_) : solid(solid_)
    {
    }

    Gkm::Solid::Model::Ptr ModelBuilder::build()
    {
        Gkm::Solid::Model::Ptr result = std::make_shared<Gkm::Solid::Model>();
        Eigen::AlignedBox3d bounding_box = solid->bbox();
        Point* x0_y0_z0 = cubes.AllocatePoint();
        x0_y0_z0->point = bounding_box.min();
        checkAndSplitCubes();
        return result;
    }
}

Gkm::Solid::Model::Ptr Gkm::Solid::buildModel(const ISolid::Ptr& solid)
{
    ModelBuilder model_buider(solid);
    return model_buider.build();
}
