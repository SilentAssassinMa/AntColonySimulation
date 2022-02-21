#include "MotionController.h"

MotionController::MotionController(uint32_t gsize, uint32_t x, uint32_t y, uint32_t n, uint32_t t, const std::string &filename)
    : ngrid_x(x), ngrid_y(y), grid_size(gsize), nants(n), sim_time(t)
{
    // init grid map
    for (uint32_t i = 0; i < ngrid_x * ngrid_y; ++i)
    {
        grid_object_map.emplace(std::make_pair(i, std::list<std::shared_ptr<Object>>()));
        grid_ant_map.emplace(std::make_pair(i, std::list<std::shared_ptr<Ant>>()));
    }

    outfile.open(filename);

    Record();
}

void MotionController::AddObject(Object::Type type, double px, double py, double radius)
{
    auto obj = std::make_shared<Object>(type, px, py, radius);
    for (auto &grid : grid_object_map)
    {
        uint32_t sy = grid.first % ngrid_y;
        uint32_t sx = (grid.first - sy) / ngrid_y;

        if (obj->IsOverlap((sx + 0.5) * grid_size, (sy + 0.5) * grid_size, grid_size))
            grid.second.emplace_back(obj);
    }

    if (type == Object::Type::Nest)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> random_angle(-3.1415926, 3.1415926);

        // generate ants from the nest
        for (uint32_t i = 0; i < nants; i++)
        {
            auto ant = std::make_shared<Ant>(px, py, random_angle(gen), ngrid_x * grid_size, ngrid_y * grid_size);
            auto [sx, sy] = Grid(px, py);
            auto grid_id = sy + sx * ngrid_y;
            ants.emplace_back(ant);
            grid_ant_map.find(grid_id)->second.emplace_back(ant);
        }
    }
}

std::vector<uint32_t> MotionController::NeighborGrids(uint32_t sx, uint32_t sy)
{
    std::vector<uint32_t> ngrids;
    ngrids.reserve(9);
    ngrids.emplace_back(sy + sx * ngrid_y);
    if (sx > 0)
    {
        ngrids.emplace_back(sy + (sx - 1) * ngrid_y);
        if (sy > 0)
            ngrids.emplace_back((sy - 1) + (sx - 1) * ngrid_y);
    }
    if (sx < ngrid_x - 1)
    {
        ngrids.emplace_back(sy + (sx + 1) * ngrid_y);
        if (sy < ngrid_y - 1)
            ngrids.emplace_back((sy + 1) + (sx + 1) * ngrid_y);
    }
    if (sy > 0)
    {
        ngrids.emplace_back((sy - 1) + sx * ngrid_y);
        if (sx < ngrid_x - 1)
            ngrids.emplace_back((sy - 1) + (sx + 1) * ngrid_y);
    }
    if (sy < ngrid_y - 1)
    {
        ngrids.emplace_back((sy + 1) + sx * ngrid_y);
        if (sx > 0)
            ngrids.emplace_back((sy + 1) + (sx - 1) * ngrid_y);
    }

    return ngrids;
}

void MotionController::Run()
{
    for (int i = 0; i < sim_time; i++)
    {
        for (auto &ant : ants)
        {
            // continue if the ant stops
            if (ant->BasicMovement())
                continue;

            auto [sx, sy] = Grid(ant->px, ant->py);

            for (auto &grid_id : NeighborGrids(sx, sy))
            {
                auto find_ants = grid_ant_map.find(grid_id);
                auto find_objects = grid_object_map.find(grid_id);
                if (find_ants == grid_ant_map.end() || find_objects == grid_object_map.end())
                {
                    std::cout << "error" << std::endl;
                    continue;
                }

                // exchange info with neighbor ants
                for (auto &neighbor_ant : find_ants->second)
                    ant->ExchangeInfo(*neighbor_ant);

                // interact with neighbor objects
                for (auto &neighbor_object : find_objects->second)
                    ant->Interact(*neighbor_object);
            }
        }

        // collide detect
        for (auto &ant : ants)
        {
            if (ant->stop)
                continue;

            auto [sx, sy] = Grid(ant->px, ant->py);

            for (auto &grid_id : NeighborGrids(sx, sy))
            {
                auto find_ants = grid_ant_map.find(grid_id);
                if (find_ants == grid_ant_map.end())
                {
                    std::cout << "error" << std::endl;
                    continue;
                }

                // avoid collision
                for (auto &neighbor_ant : find_ants->second)
                    ant->AvoidCollision(*neighbor_ant);
            }
        }

        // update the grid map
        for (auto &grid : grid_ant_map)
            grid.second.clear();

        for (auto &ant : ants)
        {
            ant->Move();
            auto [sx, sy] = Grid(ant->px, ant->py);
            auto grid_id = sy + sx * ngrid_y;
            grid_ant_map.find(grid_id)->second.emplace_back(ant);
        }
        // if (i % 25 == 0)
        Record();
    }
}

void MotionController::Record()
{
    for (size_t i = 0; i < ants.size(); i++)
        outfile << i << " " << ants[i]->px << " " << ants[i]->py << " " << ants[i]->ox << " " << ants[i]->oy << std::endl;
}