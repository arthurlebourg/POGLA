#pragma once

#include <btBulletDynamicsCommon.h>
#include <string>

#include "image.hh"
#include "image_io.hh"
#include "utils.hh"
#include "shader.hh"

class Object
{
public:
    Object(const std::string obj_file, const std::string texture,
           const glm::vec3 position, const float obj_mass);

    unsigned int get_texture();

    /*unsigned int get_VAO();

    unsigned int get_vertices_number();

    unsigned int get_indices_number();*/

    btRigidBody *get_body();

    glm::mat4 move(const glm::vec3 pos);
    
    void add_force(const glm::vec3 force);

    void add_torque(const glm::vec3 torque);

    glm::mat4 get_transform();

    void set_transform(btScalar *mat);

    glm::vec3 get_position();

    btCollisionShape *get_colShape();

    std::string tag = "";

    void draw_triangles(const Shader &shader);
    void draw_segments(const Shader &shader);

private:
    glm::vec3 position_;
    glm::mat4 transform_;
    float mass_;
    /*unsigned int VAO_;
    unsigned int EBO_;
    unsigned int vertices_number_;
    unsigned int indices_number_;*/
    unsigned int texture_id_;

    /*std::vector<glm::vec3> vertices_;
    std::vector<unsigned int> indices_;
    
    std::vector<glm::vec3> normals_;
    std::vector<glm::vec2> uv_;*/
    tifo::rgb24_image *texture_;

    std::vector<Object> children_;
    btCollisionShape *colShape_;
    btRigidBody *body_;

    Mesh mesh_triangles_;
    Mesh mesh_segments_;
};
