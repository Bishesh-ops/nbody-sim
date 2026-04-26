#include <iostream>
#include <random>
#include <SFML/Graphics.hpp>
#include "physics.hpp"
#include "quadtree.hpp"

void init_accretion_disk(nbody::ParticleSystem &p, size_t count, float width, float height)
{
    p.init(count);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<float> dist_radius(height * 0.3f, height * 0.1f);
    std::uniform_real_distribution<float> dist_angle(0.0f, 2.0f * 3.14159265f);

    float cx = width / 2.0f;
    float cy = height / 2.0f;

    for (size_t i = 0; i < count; ++i)
    {
        float r = std::abs(dist_radius(gen));
        float angle = dist_angle(gen);

        p.x[i] = cx + r * std::cos(angle);
        p.y[i] = cy + r * std::sin(angle);

        float orbital_velocity = std::sqrt(50000.0f * count / r) * 0.45f;

        p.vx[i] = -std::sin(angle) * orbital_velocity;
        p.vy[i] = std::cos(angle) * orbital_velocity;
    }
    p.x[0] = cx;
    p.y[0] = cy;
    p.vx[0] = 0.0f;
    p.vy[0] = 0.0f;
    p.mass[0] = 50000.0f;
}

int main()
{
    const unsigned int WIDTH = 1200;
    const unsigned int HEIGHT = 720;

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "N-Body Simulation: Vector Flow");
    window.setFramerateLimit(60);

    sf::View camera(sf::FloatRect(0, 0, WIDTH, HEIGHT));
    window.setView(camera);

    nbody::ParticleSystem universe;
    nbody::QuadTree tree;

    size_t particle_count = 25000;
    init_accretion_disk(universe, particle_count, WIDTH, HEIGHT);

    sf::VertexArray vertices(sf::Lines, particle_count * 2);

    float dt = 0.016f;

    std::cout << "Starting Vector Flow Simulation with " << particle_count << " particles.\n";

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseWheelScrolled)
            {
                if (event.mouseWheelScroll.delta > 0)
                    camera.zoom(0.9f);
                else
                    camera.zoom(1.1f);
            }
        }
        float pan_speed = 20.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            camera.move(0, -pan_speed);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            camera.move(0, pan_speed);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            camera.move(-pan_speed, 0);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            camera.move(pan_speed, 0);

        window.setView(camera);

        nbody::step_simulation(universe, tree, dt);

        float max_speed = 4000.0f;
        float trail_length = 0.005f;

        for (size_t i = 0; i < particle_count; ++i)
        {
            float speed = std::sqrt(universe.vx[i] * universe.vx[i] + universe.vy[i] * universe.vy[i]);
            float ratio = std::min(speed / max_speed, 1.0f);

            sf::Uint8 r = static_cast<sf::Uint8>(10 + ratio * 245);
            sf::Uint8 g = static_cast<sf::Uint8>(30 + ratio * 170);
            sf::Uint8 b = static_cast<sf::Uint8>(255 - ratio * 100);

            sf::Uint8 alpha = static_cast<sf::Uint8>(15 + ratio * 80);

            vertices[i * 2].position = sf::Vector2f(universe.x[i], universe.y[i]);
            vertices[i * 2].color = sf::Color(r, g, b, alpha);

            vertices[i * 2 + 1].position = sf::Vector2f(
                universe.x[i] - (universe.vx[i] * trail_length),
                universe.y[i] - (universe.vy[i] * trail_length));
            vertices[i * 2 + 1].color = sf::Color(r, g, b, 0);
        }

        window.clear(sf::Color(5, 5, 10));
        window.draw(vertices, sf::BlendAdd);
        window.display();
    }
    return 0;
}