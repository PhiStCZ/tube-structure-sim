#ifndef simulationclasses_hpp_
#define simulationclasses_hpp_

#include <vector>
#include "vector2f.hpp"
#include "constants.hpp"

struct color_data
{
  std::uint8_t r;
  std::uint8_t g;
  std::uint8_t b;

  color_data(std::uint8_t r, std::uint8_t g, std::uint8_t b) : r(r), g(g), b(b) {}
};

struct tube_material
{
    float density;
    float structural_force;
    float max_force;
    float break_force;
    float frictionForce;
    float velocity_resistance;
    float bounciness;
    float min_length;

    color_data color;

    tube_material(float density, float structural_force, float max_force, float break_force, float frictionForce, float velocity_resistance, float bounciness, float min_length, color_data color)
        : density(density), structural_force(structural_force), max_force(max_force), break_force(break_force), frictionForce(frictionForce), velocity_resistance(velocity_resistance), bounciness(bounciness), min_length(min_length), color(color) {}
};

struct node;
struct tube;

struct node
{
    vector2f position;
    vector2f lastPosition;
    vector2f velocity;
    vector2f force;
    vector2f moveBuffer;
    bool isFirm;
    float weight = 1;

    bool isBroken = false;

    std::vector<tube*> connectedTubes;

    node(vector2f position, bool isFirm) : position(position), lastPosition(position), isFirm(isFirm) {}
};
struct tube
{
    node* n1;
    node* n2;
    tube_material* material;
    float length;

    bool isBroken = false;

    tube(node* n1, node* n2, float length, tube_material* material) : n1(n1), n2(n2), length(length), material(material) {}
    tube(node* n1, node* n2, tube_material* material) : n1(n1), n2(n2), length(currentLength()), material(material) {}
    tube(tube_material* material) : n1(0), n2(0), length(0), material(material) {}

    float currentLength() { return vector2f::distance(n1->position, n2->position); }
    float weight() { return length * material->density; }
};


struct tube_structure
{
    std::vector<node> nodes;
    std::vector<tube> tubes;
};

// material instances
static tube_material DEFAULT_M(0.5f, 0.05f, 1.0f, 0.0035f, 0.0007f, 0.5f, 0.1f, 0.5f, { 0xFF, 0xFF, 0xFF });
static tube_material STRONG_M(0.75f, 0.25f, 1.0f, 0.015f, 0.0007f, 0.5f, 0.1f, 1.5f, { 0xCC, 0x88, 0x33 });
static tube_material SLIPPERY_M(0.5f, 0.05f, 1.0f, 0.003f, 0.00001f, 0.75f, 0.01f, 0.75f, { 0x55, 0xCC, 0xFF });
static tube_material ELASTIC_M(0.35f, 0.025f, 1.0f, 0.01f, 0.0003f, 0.02f, 0.75f, 0.3f, { 0x11, 0xDD, 0x55 });

#endif
