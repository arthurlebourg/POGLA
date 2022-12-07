#include "program.hh"

int main()
{
    std::string shd_vertex = "shaders/vertex.shd";
    std::string shd_fragment = "shaders/fragment.shd";
    std::string shd_geometry = "shaders/geometry.shd";
    std::string shd_tess_control = "shaders/tess_control.shd";
    std::string shd_tess_eval = "shaders/tess_eval.shd";

    GLFWwindow *window = init_window();

    Object plane("objects/plane.obj", "textures/white.tga", glm::vec3(0, 0, 0),
                 0.0);
    Object amogus("objects/amongus.obj", "textures/pierre_sang.tga",
                  glm::vec3(0, 30, 30), 1.0);
    Object cube("objects/cube.obj", "textures/white.tga", glm::vec3(0, 5, -5),
                0.0);

    auto scene = std::make_shared<Scene>(glm::vec3(0.0, 10.0, 0.0));
    auto player =
        std::make_shared<Player>(glm::vec3(0, 10, 0.0), glm::vec3(0, 0, 1));
    scene->add_player(player);
    scene->add_object(std::make_shared<Object>(plane));
    scene->add_object(std::make_shared<Object>(cube));
    scene->add_object(std::make_shared<Object>(amogus));

    scene->get_objs()[1]->tag = "cube";


    //std::shared_ptr<Program> prog = std::make_shared<Program>(
    //    shd_vertex, shd_fragment, shd_geometry, shd_tess_control, shd_tess_eval,
    //    window, scene);

    std::shared_ptr<Program> prog = std::make_shared<Program>(
        shd_vertex, shd_fragment, window, scene);

    set_prog_var(prog);
    if (!prog->is_ready())
    {
        std::cerr << "error building prog" << std::endl;
    }

    prog->display();

    glfwTerminate();

    return 0;
}
