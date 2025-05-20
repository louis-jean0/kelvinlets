#include <Kelvinlet.hpp>

Kelvinlet::Kelvinlet() {
    m_brush = Brush();
    computeConstants();
}

Kelvinlet::Kelvinlet(Brush brush) : m_brush(brush) {
    computeConstants();
}

Kelvinlet::~Kelvinlet() {}

void Kelvinlet::computeConstants() {
    m_a = 1.0f / (4.0f * M_PI * m_brush.mu);
    m_b = m_a / (4.0f * (1.0f - m_brush.nu));
}