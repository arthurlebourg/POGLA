#include "program.hh"
#include <climits>

double old_pos_x = 0;
double old_pos_y = 0;
int win_w = 1024;
int win_h = 1024;
bool firstMouse = true;
bool key_states[256];

float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

std::shared_ptr<Program> prog;

void set_prog_var(std::shared_ptr<Program> p)
{
    prog = p;
}

void mouse_motion_callback(GLFWwindow *, double x, double y)
{
    if (firstMouse)
    {
        old_pos_x = x;
        old_pos_y = y;
        firstMouse = false;
    }

    float xoffset = x - old_pos_x;
    float yoffset = old_pos_y - y;
    old_pos_x = x;
    old_pos_y = y;

    float sensitivity = 10.0f;
    xoffset *= deltaTime * sensitivity;
    yoffset *= deltaTime * sensitivity;

    auto scene_ = prog->get_scene();
    scene_->get_player()->add_yaw(xoffset);
    scene_->get_player()->add_pitch(yoffset);

    float dir_x = cos(glm::radians(scene_->get_player()->get_yaw()))
        * cos(glm::radians(scene_->get_player()->get_pitch()));
    float dir_y = sin(glm::radians(scene_->get_player()->get_pitch()));
    float dir_z = sin(glm::radians(scene_->get_player()->get_yaw()))
        * cos(glm::radians(scene_->get_player()->get_pitch()));

    scene_->get_player()->set_direction(glm::vec3(dir_x, dir_y, dir_z));

    scene_->get_player()->normalize_direction();

    // this is the main thing that keeps it from leaving the screen
    if (x < 50 || x > win_w - 50)
    { // you can use values other than 100 for the screen edges if you like,
      // kind of seems to depend on your mouse sensitivity for what ends up
      // working best
        old_pos_x =
            win_w / 2; // centers the last known position, this way there isn't
                       // an odd jump with your cam as it resets
        old_pos_y = win_h / 2;
        glfwSetCursorPos(prog->get_window(), win_w / 2, win_h / 2);
        // glutWarpPointer(win_w / 2, win_h / 2); // centers the cursor
    }
    else if (y < 50 || y > win_h - 50)
    {
        old_pos_x = win_w / 2;
        old_pos_y = win_h / 2;
        glfwSetCursorPos(prog->get_window(), win_w / 2, win_h / 2);
        // glutWarpPointer(win_w / 2, win_h / 2);
    }
}

void keyboard_callback(GLFWwindow *, int key, int, int action, int)
{
    if (action == GLFW_PRESS)
    {
        key_states[key] = true;
    }
    else if (action == GLFW_RELEASE)
    {
        key_states[key] = false;
    }
}

void framebuffer_size_callback(GLFWwindow *, int width, int height)
{
    win_w = width;
    win_h = height;
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void Program::display()
{
    while (!glfwWindowShouldClose(window_))
    {
        float currentFrame = glfwGetTime(); // glutGet(GLUT_ELAPSED_TIME);
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        scene_->update_physics(deltaTime);
        scene_->get_player()->set_speed(key_states['Q']);
        scene_->get_player()->move(key_states['W'] - key_states['S'],
                                   key_states['D'] - key_states['A'],
                                   deltaTime);

        render(scene_->get_player()->get_model_view(),
               scene_->get_player()->get_projection(), deltaTime);

        processInput(window_);

        glfwPollEvents();
    }
}

std::shared_ptr<Scene> Program::get_scene()
{
    return scene_;
}

GLFWwindow *init_window()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window =
        glfwCreateWindow(win_w, win_h, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return window;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return window;
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    return window;
}

Program::Program(std::string &vertex_shader_src,
                 std::string &fragment_shader_src,
                 GLFWwindow *window,
                 std::shared_ptr<Scene> scene)
    : scene_(scene)
    , window_(window)
    , render_shader_(Shader(vertex_shader_src, fragment_shader_src))
{
    ready_ = false;
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    render_shader_.use();

    glfwSetCursorPosCallback(window, mouse_motion_callback);
    glfwSetKeyCallback(window, keyboard_callback);

    ready_ = true;
}

Program::Program(std::string &vertex_shader_src, std::string &fragment_shader_src,
        std::string &geometry_shader_src, std::string &tess_control_src,
        std::string &tess_eval_src, GLFWwindow *window,
        std::shared_ptr<Scene> scene)
        : scene_(scene),
        window_(window),
        render_shader_(Shader(vertex_shader_src, fragment_shader_src,
                                geometry_shader_src, tess_control_src,
                                tess_eval_src))
{
    ready_ = false;
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    render_shader_.use();

    glfwSetCursorPosCallback(window, mouse_motion_callback);
    glfwSetKeyCallback(window, keyboard_callback);

    ready_ = true;
}

Program::~Program()
{
    glfwTerminate();
}

bool Program::is_ready()
{
    return ready_;
}

GLFWwindow *Program::get_window()
{
    return window_;
}

void Program::render(glm::mat4 const &model_view_matrix,
                     glm::mat4 const &projection_matrix, float deltaTime)
{
    glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
    glClear(
        GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    render_shader_.use();
    render_shader_.set_vec3_uniform("light_pos", scene_->get_light());
    render_shader_.set_mat4_uniform("model_view_matrix", model_view_matrix);
    render_shader_.set_mat4_uniform("projection_matrix", projection_matrix);


    if (time_to_update_seed_ <= 0.0)
    {
        time_to_update_seed_ = 1.0 / nb_of_updates_per_seconds_;
        float seed = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        render_shader_.set_float_uniform("seed", seed);
    }
    time_to_update_seed_ -= deltaTime;

    for (auto obj : scene_->get_objs())
    {
        glEnable(GL_DEPTH_TEST);TEST_OPENGL_ERROR();
        //glEnable(GL_CULL_FACE);TEST_OPENGL_ERROR();
        render_shader_.use();TEST_OPENGL_ERROR();
        glBindVertexArray(obj->get_VAO());TEST_OPENGL_ERROR();

        render_shader_.bind_texture(obj);TEST_OPENGL_ERROR();

        render_shader_.set_mat4_uniform("transform", obj->get_transform());

        //glLineWidth(5.0f);
        TEST_OPENGL_ERROR();
        glPatchParameteri(GL_PATCH_VERTICES, 4);
        glDrawElements(GL_PATCHES, obj->get_indices_number(), GL_UNSIGNED_INT, 0);
        //glDrawElements(GL_TRIANGLES_ADJACENCY, obj->get_indices_number(), GL_UNSIGNED_INT, 0);
        TEST_OPENGL_ERROR();
        
        glBindVertexArray(0);TEST_OPENGL_ERROR();
        TEST_OPENGL_ERROR();
    }

    glfwSwapBuffers(window_);
    glfwPollEvents();
}
