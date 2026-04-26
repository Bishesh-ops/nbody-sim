#include <iostream>
#include <random>
#include <SFML/Graphics.hpp>
#include "physics.hpp"
#include "quadtree.hpp"

void spawn_galaxy(nbody::ParticleSystem &p, size_t start_idx, size_t count,
                  float cx, float cy, float sys_vx, float sys_vy)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<float> dist_radius(0.0f, 150.0f); // Tighter core
    std::uniform_real_distribution<float> dist_angle(0.0f, 2.0f * 3.14159265f);

    for (size_t i = start_idx; i < start_idx + count; ++i)
    {
        float r = std::abs(dist_radius(gen));
        float angle = dist_angle(gen);

        p.x[i] = cx + r * std::cos(angle);
        p.y[i] = cy + r * std::sin(angle);

        // Orbital velocity around its own center
        float orbital_velocity = std::sqrt(50000.0f * count / (r + 50.0f)) * 0.4f;

        // Combine orbital velocity with the systemic velocity
        p.vx[i] = -std::sin(angle) * orbital_velocity + sys_vx;
        p.vy[i] = std::cos(angle) * orbital_velocity + sys_vy;
    }
}

int main()
{
    const unsigned int WIDTH = 1200;
    const unsigned int HEIGHT = 720;

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "N-Body Simulation: Collision");
    window.setFramerateLimit(60);

    nbody::ParticleSystem universe;
    nbody::QuadTree tree;

    // 10k particles PER galaxy
    size_t count_per_galaxy = 5000;
    size_t total_particles = count_per_galaxy * 2;

    universe.init(total_particles);
    spawn_galaxy(universe, 0, count_per_galaxy, WIDTH * 0.2f, HEIGHT * 0.2f, 50.0f, 30.0f);

    spawn_galaxy(universe, count_per_galaxy, count_per_galaxy, WIDTH * 0.8f, HEIGHT * 0.8f, -50.0f, -30.0f);

    sf::VertexArray vertices(sf::Points, total_particles);
    float dt = 0.016f;

    std::cout << "Starting Collision Scenario with " << total_particles << " particles.\n";

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        nbody::step_simulation(universe, tree, dt);

        float max_speed = 1800.0f;

        for (size_t i = 0; i < total_particles; ++i)
        {
            vertices[i].position = sf::Vector2f(universe.x[i], universe.y[i]);

            float speed = std::sqrt(universe.vx[i] * universe.vx[i] + universe.vy[i] * universe.vy[i]);
            float ratio = std::min(speed / max_speed, 1.0f);

            sf::Uint8 r = static_cast<sf::Uint8>(20 + ratio * (255 - 20));
            sf::Uint8 g = static_cast<sf::Uint8>(50 + ratio * (255 - 50));
            sf::Uint8 b = static_cast<sf::Uint8>(255 + ratio * (150 - 255));

            sf::Uint8 alpha = static_cast<sf::Uint8>(40 + ratio * 100);

            vertices[i].color = sf::Color(r, g, b, alpha);
        }

        window.clear(sf::Color(5, 5, 10));
        window.draw(vertices, sf::BlendAdd);

        window.display();
    }
    return 0;
}