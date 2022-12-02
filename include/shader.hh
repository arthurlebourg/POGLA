#pragma once

#include "object.hh"
#include "utils.hh"

class Shader
{
public:
    Shader(std::string &vertex_shader_src, std::string &fragment_shader_src);
    Shader(std::string &vertex_shader_src, std::string &fragment_shader_src,
           std::string &geometry_shader_src,
           std::string &tess_control_shader_src,
           std::string &tess_eval_shader_src);

    ~Shader();

    void use();

    void set_mat4_uniform(const char *name, glm::mat4 mat);
    void set_mat4_uniform(const char *name, btScalar *mat);
    void set_vec3_uniform(const char *name, glm::vec3 vec);

    void bind_texture(std::shared_ptr<Object> obj);

private:
    unsigned int shader_program_;

    char log[512];
};
