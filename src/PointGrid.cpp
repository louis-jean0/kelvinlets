#include <PointGrid.hpp>

PointGrid::PointGrid() : m_rows(10), m_cols(10), m_depth(10), m_spacing(1.0f) {
    generateGrid();
}

PointGrid::PointGrid(int rows, int cols, int depth, float spacing) : m_rows(rows), m_cols(cols), m_depth(depth), m_spacing(spacing) {
    generateGrid();
}

PointGrid::PointGrid(float spacing) : m_rows(10), m_cols(10), m_depth(10), m_spacing(spacing) {
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

void PointGrid::generateGrid() {
    clearGrid();

    // Vertices
    int midRows = m_rows / 2;
    int midCols = m_cols / 2;
    int midDepth = m_depth / 2;
    for(int i = -midRows; i < midRows; ++i) {
        for(int j = -midCols; j < midCols; ++j) {
            for(int k = -midDepth; k < midDepth; ++k) {
                float x = j * m_spacing;
                float y = i * m_spacing;
                float z = k * m_spacing;
                m_vertices.emplace_back(x,y,z);
            }
        }
    }

    // OpenGL setup
    if (m_vao) glDeleteVertexArrays(1, &m_vao);
    if (m_vbo) glDeleteBuffers(1, &m_vbo);

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(glm::vec3), m_vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void PointGrid::drawGrid() {
    glEnable(GL_PROGRAM_POINT_SIZE);
    glBindVertexArray(m_vao);
    glDrawArrays(GL_POINTS, 0, m_vertices.size());
    glBindVertexArray(0);
    glDisable(GL_PROGRAM_POINT_SIZE);
}

void PointGrid::clearGrid() {
    m_vertices.clear();
}