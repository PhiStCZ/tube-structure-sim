/**
 * @mainpage Tube Structure Simulator
 * 
 * @section Introduction
 * This program is *multipurpose*, as it is used for 2 things:
 * 
 * - learn C++
 * - learn Software development tools ;)
 * 
 * @section Implementation
 *
 * @subsection External libraries
 * The program uses **SFML** c++ library, so if you want to compile
 * it you should have the 32-bit version installed (not sure about the
 * version, but it probably doesn't matter for the purpose of this doc).
 * 
 * @subsection Process
 * The process of the program goes like this:
 * -# A `SFML::RenderWindow` for rendering is created.
 * -# A file containing the initial data is loaded (so far using the
 *    construct_sim_from_file() method), and the resulting tb_str_simulator
 *    class is linked with a new @ref renderer class
 * -# Until the end of the program:
 *   -# the tb_str_simulator simulates some amount (defined in constants.hpp)
 *      of discrete updates on itself
 *   -# the resulting tube_structure is passed to the @ref renderer, which displays
 *      it to the output window.
 */

#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include <fstream>

#include "constants.hpp"
#include "loader.hpp"
#include "renderer.hpp"
#include "simulationsystem.hpp"
#include "vector2f.hpp"

// documenting an out-of-file class...

/**
 * @class vector2f
 * @brief a class representing a vector in 2d space
 * 
 * The class contains 2 float values and several overloaded operators to make
 * working with vectors an easier task.
 */


tb_str_simulator construct_sim_from_file(const std::string& fname)
{
    std::ifstream ifs(fname);
    if (!ifs) throw std::runtime_error("could not open file");
    return loadFromInput(ifs);
}

void run(const std::string& fname) {
    auto tss = construct_sim_from_file(fname);
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), TITLE);
    renderer rdr(&window);
    sf::Clock clock;
    sf::Time last_time;

    unsigned int counter = 0;
    while (window.isOpen())
    {
        sf::Time cur_time = clock.getElapsedTime();
        if (cur_time.asSeconds() >= last_time.asSeconds() + FRAME_TIME)
        {
            last_time = cur_time;

            sf::Event e;
            while (window.pollEvent(e))
            {
                if (e.type == sf::Event::Closed) {
                    window.close();
                    return;
                }
            }

            tss.update();

            if (++counter >= UPDATES_PER_DRAW) {
                counter = 0;
                rdr.draw(tss.tb_str());
            }
        }
    }
}

int main(int argc, char* argv[])
{
    try {
        std::string fname;
        if (argc < 2) {
            std::cout << "Please enter filename of saved simulation: ";
            std::getline(std::cin, fname);
        }
        else { fname = argv[1]; }
        
        std::cout << "Running tube structure simulation from file \"" << fname << "\"" << std::endl;
        std::cout << "Close the window to terminate the program..." << std::endl;
        run(fname);
    }
    catch(std::exception e) {
        std::cout << "An error has occured: " << e.what() << std::endl;
    }
    return 0;
}
