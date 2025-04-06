#pragma once
#include "Vec2.h"
#include <SFML/Graphics/CircleShape.hpp>

class Component
{
    public:
    bool exists = false;
};

class Transform:public Component
{
    public:
    Vec2f pos = {0.0, 0.0};
    Vec2f velocity = {0.0, 0.0};
    float angle = 0.0f;

    Transform() = default;
    Transform(const Vec2f& p, const Vec2f& v, float a) : pos(p), velocity(v), angle(a){}
};

class Shape:public Component
{
public:
    sf::CircleShape circle;

    Shape() = default;
    Shape(float radius, size_t points, const sf::Color& fill, const sf::Color& outline, float thickness) : circle(radius, points)
    {
        circle.setFillColor(fill);
        circle.setOutlineColor(outline);
        circle.setOutlineThickness(thickness);
        circle.setOrigin({ radius, radius });
    }
};

class Collision : public Component
{
    public:
    float radius = 0.0f;
    Collision() = default;
    Collision(float r) : radius(r){}
};

class Score : public Component
{
    public:
    int score = 0;
    Score() = default;
    Score(int s) : score(s){}
};

class Lifespan : public Component
{
    public:
    int lifespan = 0;
    int remaining = 0;
    Lifespan() = default;
    Lifespan(int totalLifespan):lifespan(totalLifespan), remaining(totalLifespan){}
};

class Input : public Component
{
    public:
    bool up = false;
    bool left = false;
    bool right = false;
    bool down = false;
    bool shoot = false;

    Input() = default;
};