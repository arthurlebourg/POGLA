#pragma once
#include "includes.hh"


struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

    // make comparator
    
    /* bool operator<(const Vertex& other) const
    {
        / *bool pos = position.x < other.position.x || (position.x == other.position.x && position.y < other.position.y) || (position.x == other.position.x && position.y == other.position.y && position.z < other.position.z);
        bool norm = normal.x < other.normal.x || (normal.x == other.normal.x && normal.y < other.normal.y) || (normal.x == other.normal.x && normal.y == other.normal.y && normal.z < other.normal.z);
        bool uv_ = uv.x < other.uv.x || (uv.x == other.uv.x && uv.y < other.uv.y);
        bool col = color.x < other.color.x || (color.x == other.color.x && color.y < other.color.y) || (color.x == other.color.x && color.y == other.color.y && color.z < other.color.z);
        return pos || (pos == false && norm) || (pos == false && norm == false && uv_) || (pos == false && norm == false && uv_ == false && col);
        * /
        return memcmp((void*)this, (void*)&other, sizeof(Vertex)) > 0;   
    }*/
};

/*class Vertex
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
};*/