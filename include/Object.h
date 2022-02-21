#pragma once
#include <cmath>

class Object
{
public:
    enum class Type
    {
        Nest = 0,
        FoodSource = 1,
        ExternDisturb = 2,
        Obstacle = 3
    };

    Type type;
    double px;
    double py;
    double radius;

    Object(Type type_init, double px_init, double py_init, double radius_init)
        : type(type_init), px(px_init), py(py_init), radius(radius_init)
    {
    }

    bool IsInside(double x, double y)
    {
        double dist = std::sqrt(pow(x - this->px, 2) + pow(y - this->py, 2));
        return dist < radius;
    }

    // if the square is overlap with the round
    bool IsOverlap(double x, double y, double a)
    {
        return (std::fabs(px - x) <= a / 2 + radius) && (std::fabs(py - y) <= a / 2 + radius);
    }
};