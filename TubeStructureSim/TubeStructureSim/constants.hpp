#ifndef constants_hpp_
#define constants_hpp_
/**
 * @file constants.hpp
 * @author Filip Štrobl (filip.strobl@seznam.cz)
 * @brief A file containing shared program constants
 * @version 1.0
 * @date 2021-11-29
 * @copyright Copyright (c) 2021
 * 
 * This file contains all necessary constants that the program uses, including
 * constants for the main() method, the tb_str_simulator class, and the
 * renderer#draw() method.
 */
#include <string>

/**
 * @defgroup constants_main
 * @brief constants used mainly by the main() method
 * @{
 */

/** Time the program waits between processing simulation frames. */
constexpr float FRAME_TIME = (1.0f / 480);
/** How many times to update the physics before drawing. */
constexpr unsigned int UPDATES_PER_DRAW = 8;

/** Horizontal window dimension (in pixels). */
constexpr unsigned int WINDOW_WIDTH = 1024;
/** Vertical window dimension (in pixels). */
constexpr unsigned int WINDOW_HEIGHT = 768;

/** Title of the program window. */
const std::string TITLE = "Simulation in progress...";

/** @} */
/**
 * @defgroup constants_tb_str_simulator
 * @brief constants used mainly by the tb_str_simulator class
 * @{
 */

/** How much velocity a particle retains after a frame. */
constexpr float DAMPEN_CONSTANT = 0.9999f;
/** How much resistant a particle is to velocity changes. */
constexpr float VELOCITY_RESISTANCE_CONSTANT = 0.5f;
/** Default gravitational force in the horizontal direction. */
constexpr float DEFAULT_GRAVITY_X = 0.0f;
/** Default gravitational force in the vertical direction. */
constexpr float DEFAULT_GRAVITY_Y = 0.00001f;

/** How thick each tube is (when calculating collisions). */
constexpr float TUBE_THICKNESS = 0.1f;
/** Float rounding tolerance (anything below it can be considered zero). */
constexpr float EPSILON = 0.0015f;

/** @} */
/**
 * @defgroup constants_renderer_draw
 * @brief constants used mainly by the renderer#draw method
 * @{
 */

/** How much to enlarge the structures when viewing. */
constexpr float RENDER_SCALE = 32;
/** Visual radius of a point. */
constexpr float POINT_RADIUS = 2;
/** Visual radius of a firm point. */
constexpr float FIRM_POINT_RADIUS = 4;

/** @} */

#endif
