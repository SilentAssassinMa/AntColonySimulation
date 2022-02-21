#pragma once

#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <sstream>
#include <random>
#include "Object.h"

class Object;

class Ant
{
public:
    enum class State
    {
        Nothing,
        Food,
        Nest
    };

    Ant(double px_init, double py_init, double otheta_init, double boundx_init, double boundy_init);
    void Move();
    void Norm();
    bool BasicMovement();
    void ExchangeInfo(const Ant &);
    void AvoidCollision(const Ant &);
    void Interact(const Object &);

    double velocity = 1;
    double px;
    double py;
    double px_last;
    double py_last;
    double otheta;
    double ox;
    double oy;
    double boundx;
    double boundy;
    double min_dist = 100000;
    Object::Type max_type = Object::Type::Nest;

    // collide avoidance
    double w0 = 0.7; // weight that it goes in the origin direction
    double rdx = 0;
    double rdy = 0;

    State state = State::Nest;
    bool stop = false;
    uint32_t stop_time = 0;
    bool is_collecting_food = false;
    bool is_dropping_food = false;
    bool is_avoiding_obstacle = false;

    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<> random_01;
    std::uniform_real_distribution<> random_angle;
};

inline Ant::Ant(double px_init, double py_init, double otheta_init, double boundx_init, double boundy_init)
    : px(px_init), py(py_init), px_last(px_init), py_last(py_init), otheta(otheta_init),
      boundx(boundx_init), boundy(boundy_init), gen(rd()),
      random_01(0, 1), random_angle(-3.1415926 / 6, 3.1415926 / 6)
{
    ox = std::cos(otheta);
    oy = std::sin(otheta);
}

// move to the next position
inline void Ant::Move()
{
    min_dist = 100000;
    max_type = Object::Type::Nest;
    if (stop)
        return;

    // calculate the new orientation
    if (!is_avoiding_obstacle)
    {
        ox = w0 * ox + (1 - w0) * rdx;
        oy = w0 * oy + (1 - w0) * rdy;
        Norm();
    }
    rdx = 0;
    rdy = 0;

    /*
    px_last = px;
    py_last = py;
    */
    px += velocity * ox;
    py += velocity * oy;

    // bounce back at boundary
    if (px < 0)
    {
        px = -px;
        ox = -ox;
    }
    if (py < 0)
    {
        py = -py;
        oy = -oy;
    }
    if (px > boundx)
    {
        px = 2 * boundx - px;
        ox = -ox;
    }
    if (py > boundy)
    {
        py = 2 * boundy - py;
        oy = -oy;
    }

    otheta = std::atan2(oy, ox);
}

inline void Ant::Norm()
{
    double norm = std::sqrt(ox * ox + oy * oy);
    ox /= norm;
    oy /= norm;
    otheta = std::atan2(oy, ox);
}

// generate a random orientation in -Pi/6 ~ Pi/6
inline bool Ant::BasicMovement()
{
    if (stop_time == 0)
    {
        stop = false;
        if (is_collecting_food)
        {
            is_collecting_food = false;
            state = State::Food;

            ox = -ox;
            oy = -oy;
            otheta = std::atan2(oy, ox);
            return false;
        }

        if (is_dropping_food)
        {
            is_dropping_food = false;
            state = State::Nest;

            ox = -ox;
            oy = -oy;
            otheta = std::atan2(oy, ox);
            return false;
        }
    }

    if (stop == true)
    {
        stop_time--;
        return true;
    }

    /*
    if (random_01(gen) < 0.005)
    {
        stop = true;
        stop_time = 100;
        return stop;
    }
    */

    otheta += random_angle(gen);
    ox = std::cos(otheta);
    oy = std::sin(otheta);
    otheta = std::atan2(oy, ox);

    return false;
}

// exchange info with another ant
inline void Ant::ExchangeInfo(const Ant &ant)
{
    double dist = std::sqrt(pow(this->px - ant.px, 2) + pow(this->py - ant.py, 2));
    if (dist > 5)
        return;
    // change the orientation of this ant to the last position of given ant
    if ((this->state != State::Food && ant.state == State::Food) ||
        (this->state == State::Food && ant.state == State::Nest))
    {
        double target_dist = std::sqrt(pow(this->px_last - ant.px, 2) + pow(this->py_last - ant.py, 2));
        if (target_dist < min_dist)
        {
            min_dist = target_dist;
            ox = ant.px_last - this->px;
            oy = ant.py_last - this->py;
            this->Norm();
        }
    }
}

// avoid colliding with other ants
inline void Ant::AvoidCollision(const Ant &ant)
{
    if (ant.px == this->px || ant.stop)
        return;
    double px_this = px + velocity * ox;
    double py_this = py + velocity * oy;
    double px_target = ant.px + ant.velocity * ant.ox;
    double py_target = ant.py + ant.velocity * ant.oy;
    double dist = std::sqrt(pow(px_this - px_target, 2) + pow(py_this - py_target, 2));
    if (dist < 2)
    {
        rdx += (px_this - px_target) / (dist * dist);
        rdy += (py_this - py_target) / (dist * dist);
    }
}

// interact with objects
inline void Ant::Interact(const Object &object)
{
    if (this->stop)
        return;

    double dist = std::sqrt(pow(this->px - object.px, 2) + pow(this->py - object.py, 2));
    if (object.type == Object::Type::FoodSource && this->state != State::Food)
    {
        if (dist > object.radius + 10)
            return;

        if (max_type > object.type)
            return;
        else
            max_type = object.type;

        if (dist < object.radius)
        {
            state = State::Food;
            is_collecting_food = true;
            stop = true;
            stop_time = 50;
            px_last = px + 8 * random_01(gen);
            py_last = py + 8 * random_01(gen);

            return;
        }

        ox = object.px - this->px;
        oy = object.py - this->py;
        this->Norm();
    }

    if (object.type == Object::Type::Nest && this->state == State::Food)
    {
        if (dist > object.radius + 10)
            return;

        if (max_type > object.type)
            return;
        else
            max_type = object.type;

        if (dist < object.radius)
        {
            state = State::Nest;
            is_dropping_food = true;
            stop = true;
            stop_time = 50;
            px_last = px + 8 * random_01(gen);
            py_last = py + 8 * random_01(gen);

            return;
        }

        ox = object.px - this->px;
        oy = object.py - this->py;
        this->Norm();
    }

    // excape from external disturbance
    if (object.type == Object::Type::ExternDisturb)
    {
        if (dist > object.radius + 0.5)
            return;

        if (max_type > object.type)
            return;
        else
            max_type = object.type;

        ox = this->px - object.px;
        oy = this->py - object.py;
        this->Norm();
    }

    // surround the obstacles
    if (object.type == Object::Type::Obstacle)
    {
        if (dist > object.radius + 0.5)
            return;

        max_type = object.type;

        double newox = object.px - this->px;
        double newoy = object.py - this->py;
        if (newox * ox + newoy * oy < 0)
            return;

        is_avoiding_obstacle = true;
        if (random_01(gen) < 0.5)
        {
            ox = -newoy;
            oy = newox;
        }
        else
        {
            ox = newoy;
            oy = -newox;
        }
        this->Norm();
    }
}
