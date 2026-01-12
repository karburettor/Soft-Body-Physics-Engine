#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <memory>
// Configuration (Matches your Python prototype)
const float GRAVITY = 0.5f;
const float BOUNCE_DAMPING = 0.9f;
const float FRICTION = 0.99999f;
const int ITERATIONS = 50;
const int WIDTH = 800;
const int HEIGHT = 600;

struct Point {
    sf::Vector2f pos;
    sf::Vector2f old_pos;
    bool pinned = false;

    Point(float x, float y) {
        pos = {x, y};
        old_pos = {x, y};
    }

    void update() {
        if (pinned) return;

        sf::Vector2f velocity = (pos - old_pos) * FRICTION;
        old_pos = pos;
        pos += velocity;
        pos.y += GRAVITY;

        // Constraints: Screen Bounds
        if (pos.y > HEIGHT) {
            pos.y = HEIGHT;
            old_pos.y = pos.y + velocity.y * BOUNCE_DAMPING;
        }
        if (pos.x > WIDTH) {
            pos.x = WIDTH;
            old_pos.x = pos.x + velocity.x * BOUNCE_DAMPING;
        } else if (pos.x < 0) {
            pos.x = 0;
            old_pos.x = pos.x + velocity.x * BOUNCE_DAMPING;
        }
    }
};

struct Stick {
    Point* p1;
    Point* p2;
    float length;

    Stick(Point* point1, Point* point2) {
        p1 = point1;
        p2 = point2;
        float dx = p1->pos.x - p2->pos.x;
        float dy = p1->pos.y - p2->pos.y;
        length = std::sqrt(dx * dx + dy * dy);
    }

    void update() {
        sf::Vector2f delta = p1->pos - p2->pos;
        float dist = std::sqrt(delta.x * delta.x + delta.y * delta.y);
        
        if (dist == 0) return; // Prevent division by zero

        float diff = length - dist;
        float percent = (diff / dist) / 30.0f;
        
        sf::Vector2f offset = delta * percent;

        if (!p1->pinned) p1->pos += offset;
        if (!p2->pinned) p2->pos -= offset;
    }
};

class Solver {
public:
    std::vector<std::shared_ptr<Point>> points; 
    std::vector<Stick> sticks;

    void update() {
        for (auto& p : points) p->update();
        for (int i = 0; i < ITERATIONS; i++) {
            for (auto& s : sticks) s.update();
        }
    }

    void draw(sf::RenderWindow& window) {
        // Draw Sticks
        for (const auto& s : sticks) {
            sf::Vertex line[] = {
                sf::Vertex(s.p1->pos),
                sf::Vertex(s.p2->pos)
            };
            window.draw(line, 2, sf::Lines);
        }
        // Draw Points
        for (const auto& p : points) {
            sf::CircleShape circle(4);
            circle.setFillColor(sf::Color::Red);
            circle.setPosition(p->pos.x - 4, p->pos.y - 4); // Center it
            window.draw(circle);
        }
    }
    
    // Helper to add points and return reference
    Point* addPoint(float x, float y) {
        auto p = std::make_shared<Point>(x, y);
        points.push_back(p);
        return p.get(); // .get() returns the raw pointer for the Stick to use
    }
    
    void addStick(Point* p1, Point* p2) {
        sticks.emplace_back(p1, p2);
    }
};