#ifndef renderer_hpp_
#define renderer_hpp_

#include <SFML/Graphics.hpp>
#include "constants.hpp"
#include "simulationsystem.hpp"

/**
 * @brief A class for displaying the simulation on the screen
 *
 * The renderer class displays a given state of a tube_structure to a
 * RenderWindow passed to it while constructing. One renderer can output to
 * **only one RenderWindow**, but can render **multiple tube structures** (at
 * different times of course).
 * 
 * @see constants.hpp
 */
class renderer
{
    /** A window which the renderer shall render to */
    sf::RenderWindow* window;
public:
    /**
     * @brief Construct a new renderer object
     * 
     * Given a RenderWindow class, this method will construct a renderer
     * whose output will be displayed inside the given RenderWindow.
     * 
     * @param window a render window of that shall be rendered to
     */
    renderer(sf::RenderWindow* window) : window(window) { }

    /**
     * @brief Draw a single frame to the renderer's window
     * 
     * This method takes an instance tube_structure in some state, and displays
     * the structure to its renderer#window.
     * 
     * @param ts A tube_structure class representing the tube structure state
     * that shall be rendered.
     */
    void draw(const tube_structure& ts)
    {
        window->clear();
        {
            sf::VertexArray va(sf::PrimitiveType::Lines, 2);
            for (auto&& t : ts.tubes) {
                if (!t.isBroken) {
                    sf::Color c = sf::Color(t.material->color.r, t.material->color.g, t.material->color.b);
                    va[0].color = c;
                    va[1].color = c;
                    va[0].position = sf::Vector2f(t.n1->position.x * RENDER_SCALE, t.n1->position.y * RENDER_SCALE);
                    va[1].position = sf::Vector2f(t.n2->position.x * RENDER_SCALE, t.n2->position.y * RENDER_SCALE);
                    window->draw(va);
                }
            }
        }
        {
            sf::CircleShape p(POINT_RADIUS, 12);
            sf::CircleShape fp(FIRM_POINT_RADIUS, 12);
            fp.setFillColor(sf::Color(255, 100, 0, 255));
            for (auto&& n : ts.nodes) {
                
                if (n.isFirm) {
                    fp.setPosition({
                        n.position.x * RENDER_SCALE - FIRM_POINT_RADIUS,
                        n.position.y * RENDER_SCALE - FIRM_POINT_RADIUS
                    });
                    window->draw(fp);
                }
                else {
                    if (!n.isBroken) {
                        p.setPosition({
                        n.position.x * RENDER_SCALE - POINT_RADIUS,
                        n.position.y * RENDER_SCALE - POINT_RADIUS
                            });
                        window->draw(p);
                    }
                }
            }
        }
        window->display();
    }
};

#endif
