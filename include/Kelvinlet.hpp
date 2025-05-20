#pragma once

#include <glm/glm.hpp>

struct Brush {
    float epsilon = 0.1f;
    float f = 100.0f;
    float nu = 0.5f;
    float mu = 45.0f;
};

class Kelvinlet {
    public:
        Brush m_brush;
        double m_a;
        double m_b;

        Kelvinlet();
        Kelvinlet(Brush brush);
        ~Kelvinlet();

        void computeConstants();
};