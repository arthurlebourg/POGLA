#pragma once
#include "vertex.hh"

class Mesh
{
public:
    Mesh(unsigned int nb_vertex_per_patch)
    : nb_vertex_per_patch_(nb_vertex_per_patch)
    {
    }

    void load_mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
    {
        vertices_ = vertices;
        indices_ = indices;
        glGenVertexArrays(1, &VAO_);
        glGenBuffers(1, &VBO_);
        glGenBuffers(1, &EBO_);

        glBindVertexArray(VAO_);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_);

        glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex),
                     &vertices_[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int),
                     &indices_[0], GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void *)0);
        glEnableVertexAttribArray(0);
        // normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void *)offsetof(Vertex, normal_));
        glEnableVertexAttribArray(1);
        // color attribute
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void *)offsetof(Vertex, color_));
        glEnableVertexAttribArray(2);
        // uv attribute
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void *)offsetof(Vertex, uv_));
        glEnableVertexAttribArray(3);

        glBindVertexArray(0);
    }

    void draw()
    {
        glBindVertexArray(VAO_);
        glPatchParameteri(GL_PATCH_VERTICES, nb_vertex_per_patch_);
        glDrawElements(GL_PATCHES, indices_.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    std::vector<Vertex> get_vertices() const
    {
        return vertices_;
    }

private:
    unsigned int nb_vertex_per_patch_;
    std::vector<Vertex> vertices_;
    std::vector<unsigned int> indices_;
    unsigned int VAO_, VBO_, EBO_;
};