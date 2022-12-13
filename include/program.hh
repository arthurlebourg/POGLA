#pragma once

#include <fstream>

#include "image.hh"
#include "image_io.hh"
#include "player.hh"
#include "scene.hh"
#include "shader.hh"

GLFWwindow *init_window();

class Program
{
public:
    Program(GLFWwindow *window, std::shared_ptr<Scene> scene);

    ~Program();

    void use();

    void display();

    GLFWwindow *get_window();

    std::shared_ptr<Scene> get_scene();

    void update_depth_texture();

    void render(glm::mat4 const &model_view_matrix,
                glm::mat4 const &projection_matrix, float currentTime);

private:
    std::shared_ptr<Scene> scene_;

    GLFWwindow *window_;

    Shader render_shader_;
    Shader depth_shader_;

    GLuint depth_map_;
    GLuint depth_map_fbo_;

    float nb_of_updates_per_seconds_ = 12.0f; // 24 updates per seconds or 12 updates per seconds (stop motion standard)
    float time_to_update_seed_ = 1.0 / nb_of_updates_per_seconds_;
};

void set_prog_var(std::shared_ptr<Program> p);
