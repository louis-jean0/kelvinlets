#include <PointGrid.hpp>

PointGrid::PointGrid() : m_rows(100), m_cols(100), m_depth(100), m_spacing(1.0f) {
    generateGrid();
}

PointGrid::PointGrid(int rows, int cols, int depth, float spacing) : m_rows(rows), m_cols(cols), m_depth(depth), m_spacing(spacing) {
    generateGrid();
}

void PointGrid::setRows(int rows) {
    m_rows = rows;
    generateGrid();
}

void PointGrid::setCols(int cols) {
    m_cols = cols;
    generateGrid();
}

void PointGrid::setSpacing(float spacing) {
    m_spacing = spacing;
    generateGrid();
}

const std::vector<glm::vec3>& PointGrid::getVertices() const {
    return m_vertices;
}

const std::vector<unsigned int>& PointGrid::getIndices() const {
    return m_indices;
}

void PointGrid::generateGrid() {
    clearGrid();

    // Vertices
    float xOffset = -(m_cols - 1) * 0.5f * m_spacing;
    float yOffset = -(m_rows - 1) * 0.5f * m_spacing;
    float zOffset = -(m_depth - 1) * 0.5f * m_spacing;
    for(int i = 0; i < m_rows; ++i) {
        for(int j = 0; j < m_cols; ++j) {
            for(int k = 0; k < m_depth; ++k) {
                float x = j * m_spacing * xOffset;
                float y = i * m_spacing * yOffset;
                float z = k * m_spacing * zOffset;
                m_vertices.emplace_back(x,y,z);
            }
        }
    }

    // Indices
    for(int i = 0; i < m_rows - 1; ++i) {
        for(int j = 0; j < m_cols - 1; ++j) {
            for(int k = 0; k < m_depth - 1; ++k) {
                unsigned int baseIndex = (i * m_cols + j) * m_depth + k;
                m_indices.push_back(baseIndex);
                m_indices.push_back(baseIndex + 1);
                m_indices.push_back(baseIndex + m_cols);
                m_indices.push_back(baseIndex + m_cols + 1);
            }
        }
    }

    // m_vertices = {
    //     { -0.5f, -0.5f, 0.0f },
    //     {  0.5f, -0.5f, 0.0f },
    //     {  0.0f,  0.5f, 0.0f }
    // };

    // m_indices = { 0, 1, 2 };

    // OpenGL setup
    if (m_vao) glDeleteVertexArrays(1, &m_vao);
    if (m_vbo) glDeleteBuffers(1, &m_vbo);
    if (m_ebo) glDeleteBuffers(1, &m_ebo);

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(glm::vec3), m_vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void PointGrid::drawGrid() {
    glBindVertexArray(m_vao);
    glDrawElements(GL_POINTS, m_indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void PointGrid::clearGrid() {
    m_vertices.clear();
    m_indices.clear();
}