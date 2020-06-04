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
        int point_index = 0;

        bool isCube() const;
        void addNextX(Point* point);
        void addPrevX(Point* point);
        void addNextY(Point* point);
        void addPrevY(Point* point);
        void addNextZ(Point* point);
        void addPrevZ(Point* point);
        void checkEdgeX(const Point* start_x, const Point* end_x) const;
        void checkEdgeY(const Point* start_y, const Point* end_y) const;
        void checkEdgeZ(const Point* start_z, const Point* end_z) const;
        void check() const;
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
        if (prev_z)
        {
            prev_z->next_point_z = point;
            point->prev_point_z = prev_z;
        }
    }

    void Point::checkEdgeX(const Point* start_x, const Point* end_x) const
    {
        const Point* cur_point = start_x;
        while (cur_point != end_x)
        {
            assert(cur_point);
            cur_point = cur_point->next_point_x;
        }
    }

    void Point::checkEdgeY(const Point* start_y, const Point* end_y) const
    {
        const Point* cur_point = start_y;
        while (cur_point != end_y)
        {
            assert(cur_point);
            cur_point = cur_point->next_point_y;
        }
    }

    void Point::checkEdgeZ(const Point* start_z, const Point* end_z) const
    {
        const Point* cur_point = start_z;
        while (cur_point != end_z)
        {
            assert(cur_point);
            cur_point = cur_point->next_point_z;
        }
    }

    void Point::check() const
    {
        if (!isCube()) return;

        checkEdgeX(this, end_cube_x);
        checkEdgeX(end_cube_y, end_cube_xy);
        checkEdgeX(end_cube_z, end_cube_xz);
        checkEdgeX(end_cube_yz, end_cube_xyz);

        checkEdgeY(this, end_cube_y);
        checkEdgeY(end_cube_x, end_cube_xy);
        checkEdgeY(end_cube_z, end_cube_yz);
        checkEdgeY(end_cube_xz, end_cube_xyz);

        checkEdgeZ(this, end_cube_z);
        checkEdgeZ(end_cube_x, end_cube_xz);
        checkEdgeZ(end_cube_y, end_cube_yz);
        checkEdgeZ(end_cube_xy, end_cube_xyz);
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
        double TOLERANCE = 0.1;
        Gkm::Solid::ISolid::Ptr solid;
        Cubes cubes;

        Point* addPointX(Point* start_x, Point* end_x);
        Point* addPointY(Point* start_y, Point* end_y);
        Point* addPointZ(Point* start_z, Point* end_z);
        bool split(Point* cube);
        bool checkAndSplitCube(Point* cube);
        void checkAndSplitCubes();
        Gkm::Solid::Model::Ptr buildModel();

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
        new_point->point = mid;
        start_x->addNextX(new_point);
        return new_point;
    }

    Point* ModelBuilder::addPointY(Point* start_y, Point* end_y)
    {
        const Eigen::Vector3d min = start_y->point;
        const Eigen::Vector3d max = end_y->point;
        const Eigen::Vector3d sizes = max - min;
        Eigen::Vector3d mid = min;
        mid.y() += sizes.y() / 2;
        Point* cur_point = start_y;
        while (cur_point != end_y)
        {
            if (cur_point->point.y() == mid.y())
            {
                return cur_point;
            }
            cur_point = cur_point->next_point_y;
        }
        Point* new_point = cubes.AllocatePoint();
        new_point->point = mid;
        start_y->addNextY(new_point);
        return new_point;
    }

    Point* ModelBuilder::addPointZ(Point* start_z, Point* end_z)
    {
        const Eigen::Vector3d min = start_z->point;
        const Eigen::Vector3d max = end_z->point;
        const Eigen::Vector3d sizes = max - min;
        Eigen::Vector3d mid = min;
        mid.z() += sizes.z() / 2;
        Point* cur_point = start_z;
        while (cur_point != end_z)
        {
            if (cur_point->point.z() == mid.z())
            {
                return cur_point;
            }
            cur_point = cur_point->next_point_z;
        }
        Point* new_point = cubes.AllocatePoint();
        new_point->point = mid;
        start_z->addNextZ(new_point);
        return new_point;
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

        Point* p[3][3][3] = { nullptr };
        p[0][0][0] = cube;
        p[2][0][0] = cube->end_cube_x;
        p[0][2][0] = cube->end_cube_y;
        p[2][2][0] = cube->end_cube_xy;
        p[0][0][2] = cube->end_cube_z;
        p[2][0][2] = cube->end_cube_xz;
        p[0][2][2] = cube->end_cube_yz;
        p[2][2][2] = cube->end_cube_xyz;

        // Bottom
        p[1][0][0] = addPointX(cube, cube->end_cube_x);
        p[0][1][0] = addPointY(cube, cube->end_cube_y);
        p[2][1][0] = addPointY(cube->end_cube_x, cube->end_cube_xy);
        p[1][2][0] = addPointX(cube->end_cube_y, cube->end_cube_xy);
        if (!p[0][1][0]->next_point_x) p[0][1][0]->addNextX(p[2][1][0]);
        p[1][1][0] = addPointX(p[0][1][0], p[2][1][0]);
        if (!p[1][0][0]->next_point_y) p[1][0][0]->addNextY(p[1][1][0]);
        if (!p[1][1][0]->next_point_y) p[1][1][0]->addNextY(p[1][2][0]);

        // Top
        p[1][0][2] = addPointX(cube->end_cube_z, cube->end_cube_xz);
        p[0][1][2] = addPointY(cube->end_cube_z, cube->end_cube_yz);
        p[2][1][2] = addPointY(cube->end_cube_xz, cube->end_cube_xyz);
        p[1][2][2] = addPointX(cube->end_cube_yz, cube->end_cube_xyz);
        if (!p[0][1][2]->next_point_x) p[0][1][2]->addNextX(p[2][1][2]);
        p[1][1][2] = addPointX(p[0][1][2], p[2][1][2]);
        if (!p[1][0][2]->next_point_y) p[1][0][2]->addNextY(p[1][1][2]);
        if (!p[1][1][2]->next_point_y) p[1][1][2]->addNextY(p[1][2][2]);

        // Vertical links
        p[0][0][1] = addPointZ(cube, cube->end_cube_z);
        p[2][0][1] = addPointZ(cube->end_cube_x, cube->end_cube_xz);
        p[0][2][1] = addPointZ(cube->end_cube_y, cube->end_cube_yz);
        p[2][2][1] = addPointZ(cube->end_cube_xy, cube->end_cube_xyz);
        if (!p[1][0][0]->next_point_z) p[1][0][0]->addNextZ(p[1][0][2]);
        if (!p[0][1][0]->next_point_z) p[0][1][0]->addNextZ(p[0][1][2]);
        if (!p[1][1][0]->next_point_z) p[1][1][0]->addNextZ(p[1][1][2]);
        if (!p[2][1][0]->next_point_z) p[2][1][0]->addNextZ(p[2][1][2]);
        if (!p[1][2][0]->next_point_z) p[1][2][0]->addNextZ(p[1][2][2]);
        p[1][0][1] = addPointZ(p[1][0][0], p[1][0][2]);
        p[0][1][1] = addPointZ(p[0][1][0], p[0][1][2]);
        p[1][1][1] = addPointZ(p[1][1][0], p[1][1][2]);
        p[2][1][1] = addPointZ(p[2][1][0], p[2][1][2]);
        p[1][2][1] = addPointZ(p[1][2][0], p[1][2][2]);

        // Horizontal links for z1 level
        if (!p[0][0][1]->next_point_x) p[0][0][1]->addNextX(p[1][0][1]);
        if (!p[1][0][1]->next_point_x) p[1][0][1]->addNextX(p[2][0][1]);
        if (!p[0][1][1]->next_point_x) p[0][1][1]->addNextX(p[1][1][1]);
        if (!p[1][1][1]->next_point_x) p[1][1][1]->addNextX(p[2][1][1]);
        if (!p[0][2][1]->next_point_x) p[0][2][1]->addNextX(p[1][2][1]);
        if (!p[1][2][1]->next_point_x) p[1][2][1]->addNextX(p[2][2][1]);

        if (!p[0][0][1]->next_point_y) p[0][0][1]->addNextY(p[0][1][1]);
        if (!p[0][1][1]->next_point_y) p[0][1][1]->addNextY(p[0][2][1]);
        if (!p[1][0][1]->next_point_y) p[1][0][1]->addNextY(p[1][1][1]);
        if (!p[1][1][1]->next_point_y) p[1][1][1]->addNextY(p[1][2][1]);
        if (!p[2][0][1]->next_point_y) p[2][0][1]->addNextY(p[2][1][1]);
        if (!p[2][1][1]->next_point_y) p[2][1][1]->addNextY(p[2][2][1]);

        // Adjust cube end points
        for (unsigned ix = 0; ix < 2; ++ix)
        {
            for (unsigned iy = 0; iy < 2; ++iy)
            {
                for (unsigned iz = 0; iz < 2; ++iz)
                {
                    p[ix][iy][iz]->end_cube_x = p[ix + 1][iy][iz];
                    p[ix][iy][iz]->end_cube_y = p[ix][iy + 1][iz];
                    p[ix][iy][iz]->end_cube_z = p[ix][iy][iz + 1];
                    p[ix][iy][iz]->end_cube_xy = p[ix + 1][iy + 1][iz];
                    p[ix][iy][iz]->end_cube_xz = p[ix + 1][iy][iz + 1];
                    p[ix][iy][iz]->end_cube_yz = p[ix][iy + 1][iz + 1];
                    p[ix][iy][iz]->end_cube_xyz = p[ix + 1][iy + 1][iz + 1];
                    p[ix][iy][iz]->check();
                }
            }
        }

        return false;
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
            // Cube is hollow, pass it
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
            for (size_t point_index = 0; point_index < point_list.size();)
            {
                auto& point = point_list[point_index];
                if (point.isCube())
                {
                    point.check();
                    if (checkAndSplitCube(&point))
                    {
                        ++point_index;
                    }
                }
                else
                {
                    ++point_index;
                }
            }
        }
    }

    static inline Eigen::Vector3f toFloat(const Eigen::Vector3d& vector)
    {
        return Eigen::Vector3f(
            static_cast<float>(vector.x()),
            static_cast<float>(vector.y()),
            static_cast<float>(vector.z())
        );
    }

    Gkm::Solid::Model::Ptr ModelBuilder::buildModel()
    {
        Gkm::Solid::Model::Ptr result = std::make_shared<Gkm::Solid::Model>();

        size_t vertex_count = 0;
        for (auto point_list_it = cubes.points.begin(); point_list_it != cubes.points.end(); ++point_list_it)
        {
            auto& point_list = *point_list_it;
            for (auto point_it = point_list.begin(); point_it != point_list.end(); ++point_it)
            {
                auto& point = *point_it;
                if (point.isCube() && !point.hollow)
                {
                    bool prev_x_hollow = true;
                    if (point.prev_point_x) prev_x_hollow = point.prev_point_x->hollow;
                    bool next_x_hollow = true;
                    if (point.next_point_x) next_x_hollow = point.next_point_x->hollow;
                    bool prev_y_hollow = true;
                    if (point.prev_point_y) prev_y_hollow = point.prev_point_y->hollow;
                    bool next_y_hollow = true;
                    if (point.next_point_y) next_y_hollow = point.next_point_y->hollow;
                    bool prev_z_hollow = true;
                    if (point.prev_point_z) prev_z_hollow = point.prev_point_z->hollow;
                    bool next_z_hollow = true;
                    if (point.next_point_z) next_z_hollow = point.next_point_z->hollow;
                    if (prev_x_hollow) vertex_count += 6;
                    if (next_x_hollow) vertex_count += 6;
                    if (prev_y_hollow) vertex_count += 6;
                    if (next_y_hollow) vertex_count += 6;
                    if (prev_z_hollow) vertex_count += 6;
                    if (next_z_hollow) vertex_count += 6;
                }
            }
        }

        result->points.reserve(vertex_count);
        for (auto point_list_it = cubes.points.begin(); point_list_it != cubes.points.end(); ++point_list_it)
        {
            auto& point_list = *point_list_it;
            for (auto point_it = point_list.begin(); point_it != point_list.end();  ++point_it)
            {
                auto& point = *point_it;
                if (point.isCube() && !point.hollow)
                {
                    bool prev_x_hollow = true;
                    if (point.prev_point_x) prev_x_hollow = point.prev_point_x->hollow;
                    bool next_x_hollow = true;
                    if (point.next_point_x) next_x_hollow = point.next_point_x->hollow;
                    bool prev_y_hollow = true;
                    if (point.prev_point_y) prev_y_hollow = point.prev_point_y->hollow;
                    bool next_y_hollow = true;
                    if (point.next_point_y) next_y_hollow = point.next_point_y->hollow;
                    bool prev_z_hollow = true;
                    if (point.prev_point_z) prev_z_hollow = point.prev_point_z->hollow;
                    bool next_z_hollow = true;
                    if (point.next_point_z) next_z_hollow = point.next_point_z->hollow;
                    if (prev_x_hollow)
                    {
                        result->points.push_back(toFloat(point.end_cube_y->point));
                        result->points.push_back(toFloat(point.point));
                        result->points.push_back(toFloat(point.end_cube_z->point));

                        result->points.push_back(toFloat(point.end_cube_yz->point));
                        result->points.push_back(toFloat(point.end_cube_y->point));
                        result->points.push_back(toFloat(point.end_cube_z->point));
                    }
                    if (next_x_hollow)
                    {
                        result->points.push_back(toFloat(point.end_cube_x->point));
                        result->points.push_back(toFloat(point.end_cube_xy->point));
                        result->points.push_back(toFloat(point.end_cube_xz->point));

                        result->points.push_back(toFloat(point.end_cube_xz->point));
                        result->points.push_back(toFloat(point.end_cube_xy->point));
                        result->points.push_back(toFloat(point.end_cube_xyz->point));
                    }
                    if (prev_y_hollow)
                    {
                        result->points.push_back(toFloat(point.point));
                        result->points.push_back(toFloat(point.end_cube_x->point));
                        result->points.push_back(toFloat(point.end_cube_xz->point));

                        result->points.push_back(toFloat(point.point));
                        result->points.push_back(toFloat(point.end_cube_xz->point));
                        result->points.push_back(toFloat(point.end_cube_z->point));
                    }
                    if (next_y_hollow)
                    {
                        result->points.push_back(toFloat(point.end_cube_xy->point));
                        result->points.push_back(toFloat(point.end_cube_y->point));
                        result->points.push_back(toFloat(point.end_cube_yz->point));

                        result->points.push_back(toFloat(point.end_cube_xy->point));
                        result->points.push_back(toFloat(point.end_cube_yz->point));
                        result->points.push_back(toFloat(point.end_cube_xyz->point));
                    }
                    if (prev_z_hollow)
                    {
                        result->points.push_back(toFloat(point.end_cube_y->point));
                        result->points.push_back(toFloat(point.end_cube_xy->point));
                        result->points.push_back(toFloat(point.end_cube_x->point));

                        result->points.push_back(toFloat(point.point));
                        result->points.push_back(toFloat(point.end_cube_y->point));
                        result->points.push_back(toFloat(point.end_cube_x->point));
                    }
                    if (next_z_hollow)
                    {
                        result->points.push_back(toFloat(point.end_cube_z->point));
                        result->points.push_back(toFloat(point.end_cube_xz->point));
                        result->points.push_back(toFloat(point.end_cube_xyz->point));

                        result->points.push_back(toFloat(point.end_cube_z->point));
                        result->points.push_back(toFloat(point.end_cube_xyz->point));
                        result->points.push_back(toFloat(point.end_cube_yz->point));
                    }
                }
            }
        }

        return result;
    }

    ModelBuilder::ModelBuilder(const Gkm::Solid::ISolid::Ptr& solid_) : solid(solid_)
    {
    }

    Gkm::Solid::Model::Ptr ModelBuilder::build()
    {
        Eigen::AlignedBox3d bounding_box = solid->bbox();
        Eigen::Vector3d min = bounding_box.min();
        Eigen::Vector3d max = bounding_box.max();

        Point* x0_y0_z0 = cubes.AllocatePoint();
        Point* x0_y1_z0 = cubes.AllocatePoint();
        Point* x1_y0_z0 = cubes.AllocatePoint();
        Point* x1_y1_z0 = cubes.AllocatePoint();
        Point* x0_y0_z1 = cubes.AllocatePoint();
        Point* x0_y1_z1 = cubes.AllocatePoint();
        Point* x1_y0_z1 = cubes.AllocatePoint();
        Point* x1_y1_z1 = cubes.AllocatePoint();

        x0_y0_z0->point = min;
        x0_y1_z0->point = min;
        x0_y1_z0->point.y() = max.y();
        x1_y0_z0->point = min;
        x1_y0_z0->point.x() = max.x();
        x1_y1_z0->point = max;
        x1_y1_z0->point.z() = min.z();
        x0_y0_z1->point = min;
        x0_y0_z1->point.z() = max.z();
        x0_y1_z1->point = max;
        x0_y1_z1->point.x() = min.x();
        x1_y0_z1->point = max;
        x1_y0_z1->point.y() = min.y();
        x1_y1_z1->point = max;

        x0_y0_z0->addNextX(x1_y0_z0);
        x0_y1_z0->addNextX(x1_y1_z0);
        x0_y0_z1->addNextX(x1_y0_z1);
        x0_y1_z1->addNextX(x1_y1_z1);

        x0_y0_z0->addNextY(x0_y1_z0);
        x1_y0_z0->addNextY(x1_y1_z0);
        x0_y0_z1->addNextY(x0_y1_z1);
        x1_y0_z1->addNextY(x1_y1_z1);

        x0_y0_z0->addNextZ(x0_y0_z1);
        x1_y0_z0->addNextZ(x1_y0_z1);
        x0_y1_z0->addNextZ(x0_y1_z1);
        x1_y1_z0->addNextZ(x1_y1_z1);

        x0_y0_z0->end_cube_x = x1_y0_z0;
        x0_y0_z0->end_cube_y = x0_y1_z0;
        x0_y0_z0->end_cube_z = x0_y0_z1;
        x0_y0_z0->end_cube_xy = x1_y1_z0;
        x0_y0_z0->end_cube_xz = x1_y0_z1;
        x0_y0_z0->end_cube_yz = x0_y1_z1;
        x0_y0_z0->end_cube_xyz = x1_y1_z1;

        checkAndSplitCubes();
        return buildModel();
    }
}

Gkm::Solid::Model::Ptr Gkm::Solid::buildModel(const ISolid::Ptr& solid)
{
    ModelBuilder model_buider(solid);
    return model_buider.build();
}
