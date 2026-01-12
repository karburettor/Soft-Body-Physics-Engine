#include <SFML/Graphics.hpp>
#include "../include/Physics.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "C++ Soft Body Engine");
    window.setFramerateLimit(60);

    Solver solver;

    // Create Box (Note: We use pointers to connect them)
    Point* p1 = solver.addPoint(299, 100);
    Point* p2 = solver.addPoint(386, 150);
    Point* p3 = solver.addPoint(386, 250);
    Point* p4 = solver.addPoint(301, 300);
    Point* p5 = solver.addPoint(214, 250);
    Point* p6 = solver.addPoint(214, 150);

    // Box Edges
    solver.addStick(p1, p2);
    solver.addStick(p2, p3);
    solver.addStick(p3, p4);
    solver.addStick(p4, p5);
    solver.addStick(p5, p6);
    solver.addStick(p6, p1);
    

    // Cross Brace (Comment out to make it floppy)
    solver.addStick(p2, p4);
    solver.addStick(p2, p6);
    solver.addStick(p4, p6);



    // Interaction Variables
    Point* selectedPoint = nullptr;
    bool isDragging = false;

    while (window.isOpen()) {
        // Get Mouse Position relative to window
        sf::Vector2i mousePosI = sf::Mouse::getPosition(window);
        sf::Vector2f mousePos = sf::Vector2f(static_cast<float>(mousePosI.x), static_cast<float>(mousePosI.y));

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            
            // 1. Mouse Click: Find the closest point to grab
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                float minDist = 20.0f; // Grab radius
                for (auto& p : solver.points) {
                    float dx = mousePos.x - p->pos.x;
                    float dy = mousePos.y - p->pos.y;
                    float dist = std::sqrt(dx*dx + dy*dy);
                    if (dist < minDist) {
                        minDist = dist;
                        selectedPoint = p.get();
                        isDragging = true;
                    }
                }
            }

            // 2. Mouse Release: Let go
            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                isDragging = false;
                selectedPoint = nullptr;
            }

            // Right Click to Reset
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {
                p1->pos = {300, 100};p1->old_pos = {300, 100};
                p2->pos = {386, 150};p2->old_pos = {386, 150};
                p3->pos = {386, 250};p3->old_pos = {386, 250};
                p4->pos = {300, 300};p4->old_pos = {300, 300};
                p5->pos = {214, 250};p5->old_pos = {214, 250};
                p6->pos = {214, 150};p6->old_pos = {214, 150};
            }
        }

        // 3. Apply Mouse Dragging
        // We update the position, but NOT old_pos. 
        // This creates a huge velocity vector (pos - old_pos) so you can "fling" it!
        if (isDragging && selectedPoint) {
            selectedPoint->pos = mousePos;
        }

        solver.update();

        window.clear(sf::Color(30, 30, 30));
        
        // Visual Guide for Interaction
        if (isDragging) {
            sf::CircleShape mouse(10);
            mouse.setOrigin(10, 10);
            mouse.setPosition(mousePos);
            mouse.setFillColor(sf::Color(255, 255, 255, 100));
            window.draw(mouse);
        }
        
        solver.draw(window);
        window.display();
    }

    return 0;
}




