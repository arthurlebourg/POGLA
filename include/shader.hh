#pragma once

#include "utils.hh"

class Shader
{
public:
    Shader(std::string vertex_shader_src, 
           std::string fragment_shader_src);
    Shader(std::string vertex_shader_src, 
           std::string tess_control_shader_src,
           std::string tess_eval_shader_src,
           std::string geometry_shader_src,
           std::string fragment_shader_src);

    ~Shader();

    void use() const;

    void set_mat4_uniform(const char *name, glm::mat4 mat) const;
    void set_mat4_uniform(const char *name, btScalar *mat) const; 
    void set_vec2_uniform(const char *name, glm::vec2 vec) const;
    void set_vec3_uniform(const char *name, glm::vec3 vec) const;
    void set_float_uniform(const char *name, float x) const;

    //void bind_texture(std::shared_ptr<Object> obj);
    void bind_texture_depth(GLuint depth_map) const;
    void bind_texture_lines(GLuint lines_map) const;

    unsigned int shader_program_;
private:

    char log[512];
};
