#pragma once

#include <unordered_map>
#include <list>
#include <memory>
#include <string>
#include <fstream>
#include "ThreadPool.h"
#include "Ant.h"
#include "Object.h"

class MotionController
{
public:
    uint32_t ngrid_x;
    uint32_t ngrid_y;
    uint32_t grid_size;
    uint32_t nants;
    uint32_t sim_time;

    std::vector<std::shared_ptr<Ant>> ants;
    std::unordered_map<uint32_t, std::list<std::shared_ptr<Ant>>> grid_ant_map;
    std::unordered_map<uint32_t, std::list<std::shared_ptr<Object>>> grid_object_map;
    std::ofstream outfile;

    MotionController(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, const std::string &);
    void Run();
    std::vector<uint32_t> NeighborGrids(uint32_t, uint32_t);
    void AddObject(Object::Type, double, double, double);
    void Record();

    static double Dist(double px1, double py1, double px2, double py2)
    {
        return std::sqrt(pow(px1 - px2, 2) + pow(py1 - py2, 2));
    }

    std::pair<uint32_t, uint32_t> Grid(double px, double py)
    {
        return std::pair<uint32_t, uint32_t>(std::floor(px / grid_size), std::floor(py / grid_size));
    }
};
