#include <iostream>
#include <random>
#include <SFML/Graphics.hpp>
#include "physics.hpp"
#include "quadtree.hpp"

void init_galaxy(nbody::ParticleSystem &p, size_t count, float width, float height)
{
    p.init(count);
    std::random_device rd;
    std::mt19937 gen(rd());

    std::normal_distribution<float> dist_radius(0.0f, height / 3.0f);
    std::uniform_real_distribution<float> dist_angle(0.0f, 2.0f * 3.14159265f);

    float cx = width / 2.0f;
    float cy = height / 2.0f;

    for (size_t i = 0; i < count; ++i)
    {
        float r = std::abs(dist_radius(gen));
        float angle = dist_angle(gen);

        p.x[i] = cx + r * std::cos(angle);
        p.y[i] = cy + r * std::sin(angle);

        float orbital_velocity = std::sqrt(50000.0f * count / (r + 100.0f)) * 0.5f;

        p.vx[i] = -std::sin(angle) * orbital_velocity;
        p.vy[i] = std::cos(angle) * orbital_velocity;
    }
}

int main()
{
    const unsigned int WIDTH = 1200;
    const unsigned int HEIGHT = 720;

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "N-Body Simulation");
    window.setFramerateLimit(60);

    nbody::ParticleSystem universe;
    nbody::QuadTree tree;

    size_t particle_count = 10000;
    init_galaxy(universe, particle_count, WIDTH, HEIGHT);

    sf::VertexArray vertices(sf::Points, particle_count);

    float dt = 0.016f;

    std::cout << "Starting Visual Simulation with " << particle_count << " particles.\n";

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        nbody::step_simulation(universe, tree, dt);

        float max_speed = 1500.0f;

        for (size_t i = 0; i < particle_count; ++i)
        {
            vertices[i].position = sf::Vector2f(universe.x[i], universe.y[i]);

            float speed = std::sqrt(universe.vx[i] * universe.vx[i] + universe.vy[i] * universe.vy[i]);
            float ratio = std::min(speed / max_speed, 1.0f);

            sf::Uint8 r = static_cast<sf::Uint8>(30 + ratio * (255 - 30));
            sf::Uint8 g = static_cast<sf::Uint8>(80 + ratio * (255 - 80));
            sf::Uint8 b = static_cast<sf::Uint8>(255 + ratio * (200 - 255));
            sf::Uint8 alpha = static_cast<sf::Uint8>(100 + ratio * 155);

            vertices[i].color = sf::Color(r, g, b, alpha);
        }

        window.clear(sf::Color(10, 10, 15));
        window.draw(vertices);
        window.display();
    }
    return 0;
}