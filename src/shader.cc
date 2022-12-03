#include "shader.hh"
#include <string>
#include "utils.hh"

Shader::Shader(std::string &vertex_shader_src, std::string &fragment_shader_src)
{
    GLint compile_status = GL_TRUE;
    GLuint shader_id[3];
    
    shader_id[0] = glCreateShader(GL_VERTEX_SHADER);
    shader_id[1] = glCreateShader(GL_FRAGMENT_SHADER);
    shader_id[2] = glCreateShader(GL_GEOMETRY_SHADER);

    std::string vertex_shader_content = read_file(vertex_shader_src);
    std::string fragment_shader_content = read_file(fragment_shader_src);
    std::string geometry_shader_content = read_file("shaders/geometry.shd");

    char *vertex_shd_src =
        (char *)std::malloc(vertex_shader_content.length() * sizeof(char));
    char *fragment_shd_src =
        (char *)std::malloc(fragment_shader_content.length() * sizeof(char));
    char *geometry_shd_src =        
        (char *)std::malloc(geometry_shader_content.length() * sizeof(char));

    vertex_shader_content.copy(vertex_shd_src, vertex_shader_content.length());
    fragment_shader_content.copy(fragment_shd_src,
                                 fragment_shader_content.length());
    geometry_shader_content.copy(geometry_shd_src,
                                    geometry_shader_content.length());
    
    glShaderSource(shader_id[0], 1, (const GLchar **)&(vertex_shd_src), NULL);
    glShaderSource(shader_id[1], 1, (const GLchar **)&(fragment_shd_src),
                   NULL);
    glShaderSource(shader_id[2], 1, (const GLchar **)&(geometry_shd_src),
                     NULL); 

    for (int i = 0; i < 3; i++)
    {
        glCompileShader(shader_id[i]);
        TEST_OPENGL_ERROR();
        glGetShaderiv(shader_id[i], GL_COMPILE_STATUS, &compile_status);
        if (compile_status != GL_TRUE)
        {
            GLint log_size;
            char *shader_log;
            glGetShaderiv(shader_id[i], GL_INFO_LOG_LENGTH, &log_size);
            shader_log = (char *)std::malloc(
                log_size + 1); /* +1 pour le caractere de fin de chaine '\0' */
            if (shader_log != 0)
            {
                glGetShaderInfoLog(shader_id[i], log_size, &log_size,
                                   shader_log);
                std::cerr << "SHADER " << i << ": " << shader_log << std::endl;
                std::free(shader_log);
            }
            std::free(vertex_shd_src);
            std::free(fragment_shd_src);
            glDeleteShader(shader_id[0]);
            glDeleteShader(shader_id[1]);
            return ;
        }
    }
    std::free(vertex_shd_src);
    std::free(fragment_shd_src);
    std::free(geometry_shd_src);

    GLint link_status = GL_TRUE;
    shader_program_ = glCreateProgram();
    TEST_OPENGL_ERROR();
    if (shader_program_ == 0)
    {
        std::cerr << "ERROR: Impossible to create the shader program"
                  << std::endl;
        return;
    }
    for (int i = 0; i < 3; i++)
    {
        glAttachShader(shader_program_, shader_id[i]);
        TEST_OPENGL_ERROR();
    }
    glLinkProgram(shader_program_);
    TEST_OPENGL_ERROR();
    glGetProgramiv(shader_program_, GL_LINK_STATUS, &link_status);
    if (link_status != GL_TRUE)
    {
        GLint log_size;
        char *program_log;
        glGetProgramiv(shader_program_, GL_INFO_LOG_LENGTH, &log_size);
        program_log = (char *)std::malloc(
            log_size + 1); /* +1 pour le caractere de fin de chaine '\0' */
        if (program_log != 0)
        {
            glGetProgramInfoLog(shader_program_, log_size, &log_size,
                                program_log);
            std::cerr << "Program " << program_log << std::endl;
            std::free(program_log);
        }
        glDeleteProgram(shader_program_);
        TEST_OPENGL_ERROR();
        glDeleteShader(shader_id[0]);
        TEST_OPENGL_ERROR();
        glDeleteShader(shader_id[1]);
        TEST_OPENGL_ERROR();
        shader_program_ = 0;
        return;
    }
    glUseProgram(shader_program_);
    TEST_OPENGL_ERROR();
}

Shader::Shader(std::string &vertex_shader_src, std::string &fragment_shader_src,
       std::string &geometry_shader_src, std::string &tess_control_shader_src,
       std::string &tess_eval_shader_src)
{
    GLint compile_status = GL_TRUE;
    GLuint shader_id[5];
    
    shader_id[0] = glCreateShader(GL_VERTEX_SHADER);
    shader_id[1] = glCreateShader(GL_FRAGMENT_SHADER);
    shader_id[2] = glCreateShader(GL_GEOMETRY_SHADER);
    shader_id[3] = glCreateShader(GL_TESS_CONTROL_SHADER);
    shader_id[4] = glCreateShader(GL_TESS_EVALUATION_SHADER);

    std::string vertex_shader_content = read_file(vertex_shader_src);
    std::string fragment_shader_content = read_file(fragment_shader_src);
    std::string geometry_shader_content = read_file(geometry_shader_src);
    std::string tess_control_shader_content =
        read_file(tess_control_shader_src);
    std::string tess_eval_shader_content = read_file(tess_eval_shader_src);

    char *vertex_shd_src =
        (char *)std::malloc(vertex_shader_content.length() * sizeof(char));
    char *fragment_shd_src =
        (char *)std::malloc(fragment_shader_content.length() * sizeof(char));
    char *geometry_shd_src =
        (char *)std::malloc(geometry_shader_content.length() * sizeof(char));
    char *tess_control_shd_src = (char *)std::malloc(
        tess_control_shader_content.length() * sizeof(char));
    char *tess_eval_shd_src =
        (char *)std::malloc(tess_eval_shader_content.length() * sizeof(char));

    vertex_shader_content.copy(vertex_shd_src, vertex_shader_content.length());
    fragment_shader_content.copy(fragment_shd_src,
                                 fragment_shader_content.length());
    
    geometry_shader_content.copy(geometry_shd_src,
                                 geometry_shader_content.length());
    tess_control_shader_content.copy(tess_control_shd_src,
                                     tess_control_shader_content.length());
    tess_eval_shader_content.copy(tess_eval_shd_src,
                                  tess_eval_shader_content.length());

    glShaderSource(shader_id[0], 1, (const GLchar **)&(vertex_shd_src), NULL);
    glShaderSource(shader_id[1], 1, (const GLchar **)&(fragment_shd_src),
                   NULL);
    glShaderSource(shader_id[2], 1, (const GLchar **)&(geometry_shd_src),
                   NULL);
    glShaderSource(shader_id[3], 1, (const GLchar **)&(tess_control_shd_src),
                   NULL);
    glShaderSource(shader_id[4], 1, (const GLchar **)&(tess_eval_shd_src),
                   NULL);

    for (int i = 0; i < 5; i++)
    {
        glCompileShader(shader_id[i]);
        TEST_OPENGL_ERROR();
        glGetShaderiv(shader_id[i], GL_COMPILE_STATUS, &compile_status);
        if (compile_status != GL_TRUE)
        {
            GLint log_size;
            char *shader_log;
            glGetShaderiv(shader_id[i], GL_INFO_LOG_LENGTH, &log_size);
            shader_log = (char *)std::malloc(
                log_size + 1); /* +1 pour le caractere de fin de chaine '\0' */
            if (shader_log != 0)
            {
                glGetShaderInfoLog(shader_id[i], log_size, &log_size,
                                   shader_log);
                std::cerr << "SHADER " << i << ": " << shader_log << std::endl;
                std::free(shader_log);
            }
            std::free(vertex_shd_src);
            std::free(fragment_shd_src);
            std::free(tess_control_shd_src);
            std::free(tess_eval_shd_src);
            std::free(geometry_shd_src);
            glDeleteShader(shader_id[0]);
            glDeleteShader(shader_id[1]);
            glDeleteShader(shader_id[2]);
            glDeleteShader(shader_id[3]);
            glDeleteShader(shader_id[4]);
            return ;
        }
    }
    std::free(vertex_shd_src);
    std::free(fragment_shd_src);
    std::free(tess_control_shd_src);
    std::free(tess_eval_shd_src);
    std::free(geometry_shd_src);

    GLint link_status = GL_TRUE;
    shader_program_ = glCreateProgram();
    TEST_OPENGL_ERROR();
    if (shader_program_ == 0)
    {
        std::cerr << "ERROR: Impossible to create the shader program"
                  << std::endl;
        return;
    }
    for (int i = 0; i < 5; i++)
    {
        glAttachShader(shader_program_, shader_id[i]);
        TEST_OPENGL_ERROR();
    }
    glLinkProgram(shader_program_);
    TEST_OPENGL_ERROR();
    glGetProgramiv(shader_program_, GL_LINK_STATUS, &link_status);
    if (link_status != GL_TRUE)
    {
        GLint log_size;
        char *program_log;
        glGetProgramiv(shader_program_, GL_INFO_LOG_LENGTH, &log_size);
        program_log = (char *)std::malloc(
            log_size + 1); /* +1 pour le caractere de fin de chaine '\0' */
        if (program_log != 0)
        {
            glGetProgramInfoLog(shader_program_, log_size, &log_size,
                                program_log);
            std::cerr << "Program " << program_log << std::endl;
            std::free(program_log);
        }
        glDeleteProgram(shader_program_);
        TEST_OPENGL_ERROR();
        glDeleteShader(shader_id[0]);
        TEST_OPENGL_ERROR();
        glDeleteShader(shader_id[1]);
        TEST_OPENGL_ERROR();
        glDeleteShader(shader_id[2]);
        TEST_OPENGL_ERROR();
        glDeleteShader(shader_id[3]);
        TEST_OPENGL_ERROR();
        glDeleteShader(shader_id[4]);
        TEST_OPENGL_ERROR();
        shader_program_ = 0;
        return;
    }
    glUseProgram(shader_program_);
    TEST_OPENGL_ERROR();
}

Shader::~Shader()
{
    /*glDeleteShader(vertex_shader_);
    glDeleteShader(fragment_shader_);
    glDeleteShader(geometry_shader_);
    glDeleteShader(tess_control_shader_);
    glDeleteShader(tess_eval_shader_);*/
    glDeleteProgram(shader_program_);

}

void Shader::use()
{
    glUseProgram(shader_program_);
}

void Shader::set_mat4_uniform(const char *name, glm::mat4 mat)
{
    GLint location = glGetUniformLocation(shader_program_, name);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::set_mat4_uniform(const char *name, btScalar *mat)
{
    float res[16];
    for (int i = 0; i < 16; i++)
    {
        res[i] = (float)mat[i];
    }
    GLint location = glGetUniformLocation(shader_program_, name);
    glUniformMatrix4fv(location, 1, GL_FALSE, res);
}

void Shader::set_vec3_uniform(const char *name, glm::vec3 vec)
{
    GLint location = glGetUniformLocation(shader_program_, name);
    glUniform3fv(location, 1, glm::value_ptr(vec));
}

void Shader::bind_texture(std::shared_ptr<Object> obj)
{
    glBindTexture(GL_TEXTURE_2D, obj->get_texture());
    unsigned tex_location =
        glGetUniformLocation(shader_program_, "texture_sampler");
    glUniform1i(tex_location, 0);
    TEST_OPENGL_ERROR();
}
