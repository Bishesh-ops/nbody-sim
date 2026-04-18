#include <iostream>
#include "physics.hpp"

int main()
{
    std::cout << "Initializing N-Body Simulation...\n";
    nbody::ParticleSystem universe;

    size_t particle_count = 10000;
    universe.init(particle_count);

    std::cout << "Successfully allocated memory for " << universe.size() << " particles.\n";
    std::cout << "Using Data-Oriented Design (Struct of Arrays).\n";
    return 0;
}