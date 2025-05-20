#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <vector>

class PointGrid {
  public:
    PointGrid();
    PointGrid(int rows, int cols, int depth, float spacing = 1.0f);
    PointGrid(float spacing);

    void setRows(int rows);
    void setCols(int cols);
    void setDepth(int depth);
    void setSpacing(float spacing);

    void generateGrid();
    void drawGrid();

    const std::vector<glm::vec3> &getVertices() const;
    const std::vector<unsigned int> &getIndices() const;

  private:
    int m_rows;
    int m_cols;
    int m_depth;
    float m_spacing;

    std::vector<glm::vec3> m_vertices;

    GLuint m_vao;
    GLuint m_vbo;

    void clearGrid();
};