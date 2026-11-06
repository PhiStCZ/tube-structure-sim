#ifndef simulationsystem_hpp_
#define simulationsystem_hpp_

#include <vector>
#include <algorithm>

#include "constants.hpp"
#include "vector2f.hpp"
#include "simulationclasses.hpp"

/**
 * @brief A tube structure simulator
 * 
 * The class used for simulating the tube structure.
 * 
 * @see constants.hpp
 */
class tb_str_simulator
{
    tube_structure tb_str_;
    const vector2f gravity;

    std::vector<tube*> tpv;
    std::vector<node*> npv;

    void applyGravity(); 
    void recalculateStructuralForces();
    void move();
    void handleCollisions();

    void bindPerformanceContainers();
    void sortPerformanceContainers();
public:
    tb_str_simulator(tube_structure&& ts) : tb_str_(std::move(ts)), gravity(DEFAULT_GRAVITY_X, DEFAULT_GRAVITY_Y), tpv(tb_str_.tubes.size(), 0), npv(tb_str_.nodes.size(), 0)
    {
        bindPerformanceContainers();
    }

    void update();
    const tube_structure& tb_str() const { return tb_str_; }
};

class tb_str_builder
{
	enum class state { building = 0, ptr_bound = 1, invalid = 2 };
    state state_;
    tube_structure tb_str_;
    std::vector<std::pair<std::size_t, std::size_t>> tubeEndPoints_;
    
    void bindPointers();
    void calculateWeights();
public:
    tb_str_builder() :state_(state::building) {}

    void insertFirmNode(vector2f position);
    void insertTube(vector2f p1, vector2f p2, tube_material* material);
    tb_str_simulator createSimulationFromCurrentState();

    void reset() {
        state_ = state::building;
        tb_str_ = {};
        tubeEndPoints_ = {};
    }
};

#endif
