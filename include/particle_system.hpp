#pragma once
#include <vector>
#include <cstddef>

namespace nbody
{

    struct ParticleSystem
    {
        std::vector<float> x;
        std::vector<float> y;
        std::vector<float> vx;
        std::vector<float> vy;
        std::vector<float> mass;

        void init(size_t num_particles)
        {
            x.resize(num_particles, 0.0f);
            y.resize(num_particles, 0.0f);
            vx.resize(num_particles, 0.0f);
            vy.resize(num_particles, 0.0f);
            mass.resize(num_particles, 1.0f);
        }

        size_t size() const
        {
            return x.size();
        }
    };

}