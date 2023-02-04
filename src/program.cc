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
    std::cout << "framebuffer_size_callback" << std::endl;
    win_w = width;
    win_h = height;

    glViewport(0, 0, width, height);
    prog->get_scene()->get_player()->set_projection(
        glm::perspective(glm::radians(60.0f), (float)width / (float)height,
                         0.1f, 100.0f));
    prog->update_depth_texture();
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

void Program::update_depth_texture()
{
    glDeleteTextures(1, &depth_map_);
    glDeleteFramebuffers(1, &depth_map_fbo_);
    glDeleteTextures(1, &lines_map_);
    glDeleteFramebuffers(1, &lines_map_fbo_);
    TEST_OPENGL_ERROR();

    GLint m_viewport[4];
    glGetIntegerv(GL_VIEWPORT, m_viewport);
    TEST_OPENGL_ERROR();
    
    glGenFramebuffers(1, &depth_map_fbo_);TEST_OPENGL_ERROR();

    glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo_);TEST_OPENGL_ERROR();
    glGenTextures(1, &depth_map_);TEST_OPENGL_ERROR();

    glBindTexture(GL_TEXTURE_2D, depth_map_);TEST_OPENGL_ERROR();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_viewport[2], m_viewport[3], 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, depth_map_, 0);
    // attach depth texture as FBO's depth buffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                           depth_map_, 0);
    
    unsigned int rbo_depth;
    glGenRenderbuffers(1, &rbo_depth);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_viewport[2], m_viewport[3]); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo_depth); // now actually attach it
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    TEST_OPENGL_ERROR();

    glGenFramebuffers(1, &lines_map_fbo_);TEST_OPENGL_ERROR();
    glBindFramebuffer(GL_FRAMEBUFFER, lines_map_fbo_);TEST_OPENGL_ERROR();
    glGenTextures(1, &lines_map_);TEST_OPENGL_ERROR();

    glBindTexture(GL_TEXTURE_2D, lines_map_);TEST_OPENGL_ERROR();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_viewport[2], m_viewport[3], 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lines_map_, 0);
    // attach depth texture as FBO's depth buffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                           lines_map_, 0);
    
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_viewport[2], m_viewport[3]); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    TEST_OPENGL_ERROR();
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
        glfwCreateWindow(win_w, win_h, "Poglamogus", NULL, NULL);
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


Program::Program(GLFWwindow *window,
        std::shared_ptr<Scene> scene)
        : scene_(scene)
        , window_(window)
        , lines_shader_(Shader("shaders/classic.vert", "shaders/classic.tesc",
                                "shaders/classic.tese", "shaders/classic.geom",
                                "shaders/classic.frag"))
        , depth_shader_(Shader("shaders/depth.vert", "shaders/depth.tesc",
                               "shaders/depth.tese", "shaders/depth.geom",
                               "shaders/depth.frag"))
        , quad_shader_(Shader("shaders/quad.vert", "shaders/quad.frag"))
{
    lines_shader_.use();TEST_OPENGL_ERROR();
    depth_shader_.use();

    glfwSetCursorPosCallback(window, mouse_motion_callback);
    glfwSetKeyCallback(window, keyboard_callback);

    update_depth_texture();

    // create quad for rendering depth texture
    // ---------------------------------------

    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    // screen quad VAO
    glGenVertexArrays(1, &quadVAO_);
    glGenBuffers(1, &quadVBO_);
    glBindVertexArray(quadVAO_);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

Program::~Program()
{
    glfwTerminate();
}

GLFWwindow *Program::get_window()
{
    return window_;
}

void Program::render(glm::mat4 const &model_view_matrix,
                     glm::mat4 const &projection_matrix, float deltaTime)
{
    lines_shader_.use();TEST_OPENGL_ERROR();
    //lines_shader_.set_vec3_uniform("light_pos", scene_->get_light());
    lines_shader_.set_mat4_uniform("model_view_matrix", model_view_matrix);TEST_OPENGL_ERROR();
    lines_shader_.set_mat4_uniform("projection_matrix", projection_matrix);TEST_OPENGL_ERROR();
    depth_shader_.use();TEST_OPENGL_ERROR();
    depth_shader_.set_mat4_uniform("model_view_matrix", model_view_matrix);TEST_OPENGL_ERROR();
    depth_shader_.set_mat4_uniform("projection_matrix", projection_matrix);TEST_OPENGL_ERROR();
    
    GLint m_viewport[4];

    glGetIntegerv(GL_VIEWPORT, m_viewport);
    
    lines_shader_.use();TEST_OPENGL_ERROR();
    lines_shader_.set_vec2_uniform("viewSize", glm::vec2(m_viewport[2], m_viewport[3]));
    TEST_OPENGL_ERROR();


    if (time_to_update_seed_ <= 0.0)
    {
        time_to_update_seed_ = 1.0 / nb_of_updates_per_seconds_;
        float seed = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

        lines_shader_.use();TEST_OPENGL_ERROR();
        lines_shader_.set_float_uniform("seed", seed);TEST_OPENGL_ERROR();
        
        depth_shader_.use();TEST_OPENGL_ERROR();
        depth_shader_.set_float_uniform("seed", seed);TEST_OPENGL_ERROR();
    }
    time_to_update_seed_ -= deltaTime;
    TEST_OPENGL_ERROR();

    // depth render
    depth_shader_.use();TEST_OPENGL_ERROR();
    glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo_);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    TEST_OPENGL_ERROR();
    glDepthFunc(GL_LESS);
    TEST_OPENGL_ERROR();
    glEnable(GL_CULL_FACE);
    TEST_OPENGL_ERROR();
    for (auto obj : scene_->get_objs())
    {
        obj->draw_triangles(depth_shader_);
    }
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // lines render
    lines_shader_.use();TEST_OPENGL_ERROR();
    glBindFramebuffer(GL_FRAMEBUFFER, lines_map_fbo_);
    //glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
    glClear(
        GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    TEST_OPENGL_ERROR();
    glDepthFunc(GL_LESS); 
    TEST_OPENGL_ERROR();
    //lines_shader_.bind_texture_depth(depth_map_);TEST_OPENGL_ERROR();
    glLineWidth(5.0f);
    glDisable(GL_CULL_FACE);
    for (auto obj : scene_->get_objs())
    {
        obj->draw_segments(lines_shader_);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glEnable(GL_CULL_FACE);

    // render
    quad_shader_.use();TEST_OPENGL_ERROR();
    //glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    TEST_OPENGL_ERROR();
    glDepthFunc(GL_LESS);
    TEST_OPENGL_ERROR();
    quad_shader_.bind_texture_depth(depth_map_);TEST_OPENGL_ERROR();
    quad_shader_.bind_texture_lines(lines_map_);TEST_OPENGL_ERROR();
    glBindVertexArray(quadVAO_);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    TEST_OPENGL_ERROR();


    glfwSwapBuffers(window_);
    glfwPollEvents();
}
