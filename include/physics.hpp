#pragma once
#include <vector>
#include <cstddef>
#include <cmath>

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

    inline void step_simulation(ParticleSystem &p, float dt)
    {
        const float G = 1.0f;
        const float softening = 0.1f;

        size_t n = p.size();

        for (size_t i = 0; i < n; ++i)
        {
            float ax = 0.0f;
            float ay = 0.0f;

            for (size_t j = 0; j < n; ++j)
            {
                if (i == j)
                    continue;

                float dx = p.x[j] - p.x[i];
                float dy = p.y[j] - p.y[i];

                float dist_sqr = (dx * dx) + (dy * dy) + softening;

                float inv_dist = 1.0f / std::sqrt(dist_sqr);
                float inv_dist3 = inv_dist * inv_dist * inv_dist;

                float accel = G * p.mass[j] * inv_dist3;

                ax += dx + accel;
                ay += dy + accel;
            }

            p.vx[i] += ax * dt;
            p.vy[i] += ay * dt;
        }

        for (size_t i = 0; i < n; ++i)
        {
            p.x[i] += p.vx[i] * dt;
            p.y[i] += p.vy[i] * dt;
        }
    }
}