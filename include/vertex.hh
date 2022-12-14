#pragma once
#include "includes.hh"

class Vertex
{
public:
    Vertex(glm::vec3 pos, glm::vec3 normal, glm::vec3 color, glm::vec2 uv)
    : pos_(pos), normal_(normal), color_(color), uv_(uv)
    {
    }

    Vertex()
    : pos_(glm::vec3(0.0f, 0.0f, 0.0f)), normal_(glm::vec3(0.0f, 0.0f, 0.0f)), color_(glm::vec3(0.0f, 0.0f, 0.0f)), uv_(glm::vec2(0.0f, 0.0f))
    {
    }

    bool operator<(const Vertex& other) const
    {
        return memcmp((void*)this, (void*)&other, sizeof(Vertex)) > 0;   
    }

    std::vector<float> get_data()
    {
        std::vector<float> data;
        data.push_back(pos_.x);
        data.push_back(pos_.y);
        data.push_back(pos_.z);
        data.push_back(normal_.x);
        data.push_back(normal_.y);
        data.push_back(normal_.z);
        data.push_back(color_.x);
        data.push_back(color_.y);
        data.push_back(color_.z);
        data.push_back(uv_.x);
        data.push_back(uv_.y);
        return data;
    }

    glm::vec3 pos_;
    glm::vec3 normal_;
    glm::vec3 color_;
    glm::vec2 uv_;
};