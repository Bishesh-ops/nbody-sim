#include <iostream>
#include "physics.hpp"

int main()
{
    std::cout << "Initializing N-Body Simulation...\n";
    nbody::ParticleSystem universe;

    size_t particle_count = 1000;
    universe.init(particle_count);

    universe.x[1] = 10.0f;
    universe.y[1] = 10.0f;

    float dt = 0.016f;

    std::cout << "Starting Simulation now. \n";

    for (int i = 0; i < 50; ++i)
    {
        nbody::step_simulation(universe, dt);

        std::cout << "Frame " << i
                  << "| Particle 0 Position: ("
                  << universe.x[0] << ", " << universe.y[0] << ")\n";
    }
    return 0;
}