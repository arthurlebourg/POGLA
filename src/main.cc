#include "program.hh"

int main()
{
    GLFWwindow *window = init_window();

    Object sce("objects/casa_color.obj", "textures/white.tga", glm::vec3(0, -20, 0), 0.0);
    
    Object plane("objects/plane.obj", "textures/white.tga", glm::vec3(0, -35, 0), 0.0);
    

    auto scene = std::make_shared<Scene>(glm::vec3(0.0, 10.0, 0.0));
    auto player =
        std::make_shared<Player>(glm::vec3(0, 10, 0.0), glm::vec3(0, 0, 1));
    scene->add_player(player);
    scene->add_object(std::make_shared<Object>(plane));
    scene->add_object(std::make_shared<Object>(sce));
    for (int i = 0; i < 2; i++)
    {
        Object amogus("objects/amongus.obj", "textures/pierre_sang.tga",
                      glm::vec3(0, 30 + 2*i, 30), 1.0);
        Object voiture("objects/car.obj", "textures/white.tga", glm::vec3(100 + 5 * i, 10, 0), 1.0);
        scene->add_object(std::make_shared<Object>(amogus));
        scene->add_object(std::make_shared<Object>(voiture));
    }

    std::cout << "All objects loaded" << std::endl;

    //scene->get_objs()[1]->tag = "cube";


    std::shared_ptr<Program> prog = std::make_shared<Program>(window, scene);

    set_prog_var(prog);
    prog->display();

    glfwTerminate();

    return 0;
}
