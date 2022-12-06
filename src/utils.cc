#include "utils.hh"

void test_opengl_error(std::string func, std::string file, int line)
{
    GLenum err = glGetError();
    switch (err)
    {
    case GL_NO_ERROR:
        return;
    case GL_INVALID_ENUM:
        std::cerr << file << ":" << line << "(" << func << ") ";
        std::cerr << "GL_INVALID_ENUM\n";
        break;
    case GL_INVALID_VALUE:
        std::cerr << file << ":" << line << "(" << func << ") ";
        std::cerr << "GL_INVALID_VALUE\n";
        break;
    case GL_INVALID_OPERATION:
        std::cerr << file << ":" << line << "(" << func << ") ";
        std::cerr << "GL_INVALID_OPERATION\n";
        break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        std::cerr << file << ":" << line << "(" << func << ") ";
        std::cerr << "GL_INVALID_FRAMEBUFFER_OPERATION\n";
        break;
    case GL_OUT_OF_MEMORY:
        std::cerr << file << ":" << line << "(" << func << ") ";
        std::cerr << "GL_OUT_OF_MEMORY\n";
        break;
    case GL_STACK_UNDERFLOW:
        std::cerr << file << ":" << line << "(" << func << ") ";
        std::cerr << "GL_STACK_UNDERFLOW\n";
        break;
    case GL_STACK_OVERFLOW:
        std::cerr << file << ":" << line << "(" << func << ") ";
        std::cerr << "GL_STACK_OVERFLOW\n";
        break;
    default:
        std::cerr << file << ":" << line << "(" << func << ") ";
        std::cerr << "UNKONWN ERROR\n";
        break;
    }
}

void load_obj(const char *filename, std::vector<glm::vec3> &vertices,
              std::vector<glm::vec2> &uvs, std::vector<glm::vec3> &normals)
{
    std::ifstream in(filename, std::ios::in);
    if (!in)
    {
        std::cerr << "Cannot open " << filename << std::endl;
        return;
    }
    std::vector<unsigned int> vertexIndices;
    std::vector<unsigned int> uvIndices;
    std::vector<unsigned int> normalIndices;

    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec3> temp_normals;
    std::vector<glm::vec2> temp_uv;

    std::string line("");
    while (getline(in, line))
    {
        if (line.substr(0, 2) == "v ")
        {
            std::istringstream s(line.substr(2));
            glm::vec3 v;
            s >> v.x;
            s >> v.y;
            s >> v.z;
            temp_vertices.push_back(v);
        }
        else if (line.substr(0, 2) == "vt")
        {
            std::istringstream s(line.substr(2));
            glm::vec2 v;
            s >> v.x;
            s >> v.y;
            temp_uv.push_back(v);
        }
        else if (line.substr(0, 2) == "vn")
        {
            std::istringstream s(line.substr(2));
            glm::vec3 v;
            s >> v.x;
            s >> v.y;
            s >> v.z;
            temp_normals.push_back(v);
        }
        else if (line.substr(0, 2) == "f ")
        {
            std::string s(line.substr(2));
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            //unsigned int vertexIndex[4], uvIndex[4], normalIndex[4];
            int matches = sscanf(s.c_str(), "%u/%u/%u %u/%u/%u %u/%u/%u \n",
                                 &vertexIndex[0], &uvIndex[0], &normalIndex[0],
                                 &vertexIndex[1], &uvIndex[1], &normalIndex[1],
                                 &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
                                 //&vertexIndex[3], &uvIndex[3], &normalIndex[3]);
            if (matches != 9)
            //if (matches != 12)
            {
                printf("File can't be read by our simple parser : ( Try "
                       "exporting with other options\n");
                printf("Please use quads encoding\n");
                return;
            }
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            //vertexIndices.push_back(vertexIndex[3]);
            uvIndices.push_back(uvIndex[0]);
            uvIndices.push_back(uvIndex[1]);
            uvIndices.push_back(uvIndex[2]);
            //uvIndices.push_back(uvIndex[3]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
            //normalIndices.push_back(normalIndex[3]);
        }
        /* anything else is ignored */
    }
    
    //  Build a hash table that maps from a half-edge (expressed as a pair of indices, in order) to the third index of the triangle the half-edge belongs to.
    std::map<std::pair<unsigned int, unsigned int>, unsigned int> halfEdgeToTriangle;
    for (unsigned int i = 0; i < vertexIndices.size(); i += 3)
    {
        halfEdgeToTriangle[std::make_pair(vertexIndices[i], vertexIndices[i + 1])] = vertexIndices[i + 2];
        halfEdgeToTriangle[std::make_pair(vertexIndices[i + 1], vertexIndices[i + 2])] = vertexIndices[i];
        halfEdgeToTriangle[std::make_pair(vertexIndices[i + 2], vertexIndices[i])] = vertexIndices[i + 1];
    }

    //  For each half-edge, find the other half-edge that shares the same triangle.
    std::map<std::pair<unsigned int, unsigned int>, std::pair<unsigned int, unsigned int>> halfEdgeToHalfEdge;
    for (auto it = halfEdgeToTriangle.begin(); it != halfEdgeToTriangle.end(); ++it)
    {
        unsigned int v1 = it->first.first;
        unsigned int v2 = it->first.second;
        unsigned int v3 = it->second;
        halfEdgeToHalfEdge[it->first] = std::make_pair(v3, v1);
        halfEdgeToHalfEdge[std::make_pair(v3, v1)] = std::make_pair(v2, v3);
        halfEdgeToHalfEdge[std::make_pair(v2, v3)] = std::make_pair(v1, v2);
    }

    for (unsigned int i = 0; i < vertexIndices.size(); i+=3)
    {
        unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
        vertexIndex[0] = vertexIndices[i];
        vertexIndex[1] = vertexIndices[i+1];
        vertexIndex[2] = vertexIndices[i+2];
        uvIndex[0] = uvIndices[i];
        uvIndex[1] = uvIndices[i+1];
        uvIndex[2] = uvIndices[i+2];
        normalIndex[0] = normalIndices[i];
        normalIndex[1] = normalIndices[i+1];
        normalIndex[2] = normalIndices[i+2];

        for (unsigned int j = 0; j < 3; j++)
        {
            //first vertex
            glm::vec3 vertex = temp_vertices[vertexIndex[j]-1];
            vertices.push_back(vertex);

            // get half edge
            std::pair<unsigned int, unsigned int> halfEdge = std::make_pair(vertexIndex[j], vertexIndex[(j+1)%3]);
            std::pair<unsigned int, unsigned int> otherHalfEdge = halfEdgeToHalfEdge[halfEdge];

            // adjacent vertex
            unsigned int adjacentVertex_index = halfEdgeToTriangle[otherHalfEdge];
            glm::vec3 adjacentVertex = temp_vertices[adjacentVertex_index-1];
            vertices.push_back(adjacentVertex);

            
            glm::vec2 uv = temp_uv[uvIndex[j]-1];
            uvs.push_back(uv);
            uvs.push_back(temp_vertices[adjacentVertex_index-1]);


            glm::vec3 normal = temp_normals[normalIndex[j]-1];
            normals.push_back(normal);
            normals.push_back(temp_normals[adjacentVertex_index-1]);
        }
        /*
        unsigned int vertexIndex = vertexIndices[i];
        glm::vec3 vertex = temp_vertices[vertexIndex - 1];
        vertices.push_back(vertex);*/
    }
    /*
    for (unsigned int i = 0; i < normalIndices.size(); i++)
    {
        unsigned int normalIndex = normalIndices[i];
        glm::vec3 vertex = temp_normals[normalIndex - 1];
        normals.push_back(vertex);
    }
    for (unsigned int i = 0; i < uvIndices.size(); i++)
    {
        unsigned int uvIndex = uvIndices[i];
        glm::vec2 vertex = temp_uv[uvIndex - 1];
        uv.push_back(vertex);
    }*/
}

std::string read_file(const std::string &filename)
{
    std::ifstream input_src_file(filename, std::ios::in);
    std::string ligne;
    std::string file_content = "";
    if (input_src_file.fail())
    {
        std::cerr << "FAIL\n";
        return "";
    }
    while (getline(input_src_file, ligne))
    {
        file_content = file_content + ligne + "\n";
    }
    file_content += '\0';
    input_src_file.close();
    return file_content;
}

/*void set_mat4_uniform(unsigned int shader_program, const char *name,
                      glm::mat4 mat)
{
    GLint location = glGetUniformLocation(shader_program, name);
    TEST_OPENGL_ERROR();
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
    TEST_OPENGL_ERROR();
}

void set_mat4_uniform(unsigned int shader_program, const char *name,
                      btScalar *mat)
{
    float res[16];
    for (int i = 0; i < 16; i++)
    {
        res[i] = (float)mat[i];
    }
    GLint location = glGetUniformLocation(shader_program, name);
    TEST_OPENGL_ERROR();
    glUniformMatrix4fv(location, 1, GL_FALSE, res);
    TEST_OPENGL_ERROR();
}

void set_vec3_uniform(unsigned int shader_program, const char *name,
                      glm::vec3 vec)
{
    GLint location = glGetUniformLocation(shader_program, name);
    TEST_OPENGL_ERROR();
    glUniform3fv(location, 1, glm::value_ptr(vec));
    TEST_OPENGL_ERROR();
}*/

btVector3 glmToBullet(const glm::vec3 &v)
{
    return btVector3(v.x, v.y, v.z);
}

btMatrix3x3 glmToBullet(const glm::mat3 &m)
{
    return btMatrix3x3(m[0][0], m[1][0], m[2][0], m[0][1], m[1][1], m[2][1],
                       m[0][2], m[1][2], m[2][2]);
}

btTransform glmToBullet(const glm::mat4 &m)
{
    glm::mat3 m3(m);
    return btTransform(glmToBullet(m3),
                       glmToBullet(glm::vec3(m[3][0], m[3][1], m[3][2])));
}

glm::vec3 get_vector(const glm::vec3 point_src, const glm::vec3 point_dest,
                     float angle)
{
    angle = glm::radians(angle);
    auto vec = point_dest - point_src;
    // rotation around y
    auto rotationM = glm::mat3(cos(angle), 0, sin(angle), 0, 1, 0, -sin(angle),
                               0, cos(angle));
    return rotationM * vec;
}

glm::vec3 find_normale(const glm::vec3 point_src, const glm::vec3 point_dest1,
                       const glm::vec3 point_dest2, float angle)
{
    auto vec1 = get_vector(point_src, point_dest1, angle);
    auto vec2 = get_vector(point_src, point_dest2, angle);
    auto res = glm::normalize(glm::cross(vec1, vec2));
    res.x = -res.x;
    return res;
}

float reset_angle(float angle_to_be_set, const float angle)
{
    if (angle_to_be_set > angle)
        angle_to_be_set -= angle;
    if (angle_to_be_set < -angle)
        angle_to_be_set += angle;
    return angle_to_be_set;
}

bool is_sameSign(const float f1, const float f2)
{
    if ((f1 > 0 && f2 < 0) || (f1 < 0 && f2 > 0))
    {
        return false;
    }
    return true;
}
