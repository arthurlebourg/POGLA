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
              std::vector<glm::vec2> &uvs, std::vector<glm::vec3> &normals, 
              std::vector<unsigned int> &indices, std::vector<float> &vbo_data)
{
    std::ifstream file(filename);
    std::string line;
    std::vector<unsigned int> vertexIndices;
    std::vector<unsigned int> uvIndices;
    std::vector<unsigned int> normalIndices;

    std::map<std::pair<unsigned int, unsigned int>, unsigned int> halfEdgeToVertex;
    std::map<std::pair<unsigned int, unsigned int>, unsigned int> halfEdgeToNormal;
    std::map<std::pair<unsigned int, unsigned int>, unsigned int> halfEdgeToUv;
    
    while (file >> line)
    {
        GLfloat v1;
        GLfloat v2;
        GLfloat v3;
        if (!line.compare("v"))
        {
            file >> v1 >> v2 >> v3;
            vertices.push_back(glm::vec3(v1, v2, v3));
        }
        else if (!line.compare("vt"))
        {
            file >> v1 >> v2;
            uvs.push_back(glm::vec2(v1, v2));
        }
        else if (!line.compare("vn"))
        {
            file >> v1 >> v2 >> v3;
            normals.push_back(glm::vec3(v1, v2, v3));
        }
        else if (!line.compare("f"))
        {
            char slash; // unused
            size_t v1, vn1, vt1, v2, vn2, vt2, v3, vn3, vt3;
            file >> v1 >> slash >> vt1 >> slash >> vn1;
            v1--;
            vt1--;
            vn1--;
            file >> v2 >> slash >> vt2 >> slash >> vn2;
            v2--;
            vt2--;
            vn2--;
            file >> v3 >> slash >> vt3 >> slash >> vn3;
            v3--;
            vt3--;
            vn3--;
            vertexIndices.push_back(v1);
            vertexIndices.push_back(v2);
            vertexIndices.push_back(v3);
            uvIndices.push_back(vt1);
            uvIndices.push_back(vt2);
            uvIndices.push_back(vt3);
            normalIndices.push_back(vn1);
            normalIndices.push_back(vn2);
            normalIndices.push_back(vn3);
            halfEdgeToVertex[std::make_pair(v1, v2)] = v3;
            halfEdgeToNormal[std::make_pair(v1, v2)] = vn3;
            halfEdgeToUv[std::make_pair(v1, v2)] = vt3;

            halfEdgeToVertex[std::make_pair(v2, v3)] = v1;
            halfEdgeToNormal[std::make_pair(v2, v3)] = vn1;
            halfEdgeToUv[std::make_pair(v2, v3)] = vt1;

            halfEdgeToVertex[std::make_pair(v3, v1)] = v2;
            halfEdgeToNormal[std::make_pair(v3, v1)] = vn2;
            halfEdgeToUv[std::make_pair(v3, v1)] = vt2;
        }
    }
    
    std::map<std::array<unsigned int, 4>, unsigned int> pushed_vertices;
    std::map<std::pair<unsigned int, unsigned int>, unsigned int> pushed_edges;

    int counter_index = 0;
    for (unsigned int i = 0; i < vertexIndices.size(); i++)
    {
        unsigned int second_index = i + 1;
        unsigned int is_base_triangle = i % 3 == 0 ? 1 : 0;
        if (i % 3 == 2)
        {
            second_index = i - 2;
        }
        if (pushed_edges.find({ vertexIndices[i], vertexIndices[second_index] }) != pushed_edges.end())
        {
            //continue;
        }
        else
        {
            pushed_edges[std::make_pair(vertexIndices[i], vertexIndices[second_index])] = counter_index;
            pushed_edges[std::make_pair(vertexIndices[second_index], vertexIndices[i])] = counter_index;
        }

        std::pair<unsigned int, unsigned int> left_edge =
            std::make_pair(vertexIndices[second_index], vertexIndices[i]); // left is the adjacent vertex

        std::pair<unsigned int, unsigned int> right_edge =
            std::make_pair(vertexIndices[i], vertexIndices[second_index]);

        unsigned int left_vertex_index = halfEdgeToVertex[left_edge];
        unsigned int left_normal_index = halfEdgeToNormal[left_edge];
        unsigned int left_uv_index = halfEdgeToUv[left_edge];
        
        unsigned int right_vertex_index = halfEdgeToVertex[right_edge];
        unsigned int right_normal_index = halfEdgeToNormal[right_edge];
        unsigned int right_uv_index = halfEdgeToUv[right_edge];

        if (pushed_vertices.find({ vertexIndices[i], normalIndices[i], uvIndices[i], is_base_triangle })
            == pushed_vertices.end())
        {
            vbo_data.push_back(vertices[vertexIndices[i]].x);
            vbo_data.push_back(vertices[vertexIndices[i]].y);
            vbo_data.push_back(vertices[vertexIndices[i]].z);
            vbo_data.push_back(normals[normalIndices[i]].x);
            vbo_data.push_back(normals[normalIndices[i]].y);
            vbo_data.push_back(normals[normalIndices[i]].z);
            vbo_data.push_back(uvs[uvIndices[i]].x);
            vbo_data.push_back(uvs[uvIndices[i]].y);
            vbo_data.push_back(is_base_triangle == 1 ? 1.0f : -1.0f);

            indices.push_back(counter_index);
            pushed_vertices[{ vertexIndices[i], normalIndices[i], uvIndices[i], is_base_triangle }] =
                counter_index++;
        }
        else
        {
            indices.push_back(pushed_vertices[{ vertexIndices[i], normalIndices[i],
                                                uvIndices[i], is_base_triangle }]);
        }

        if (pushed_vertices.find({ vertexIndices[second_index],
                                   normalIndices[second_index],
                                   uvIndices[second_index],
                                   is_base_triangle }) == pushed_vertices.end())
        {
            vbo_data.push_back(vertices[vertexIndices[second_index]].x);
            vbo_data.push_back(vertices[vertexIndices[second_index]].y);
            vbo_data.push_back(vertices[vertexIndices[second_index]].z);
            vbo_data.push_back(normals[normalIndices[second_index]].x);
            vbo_data.push_back(normals[normalIndices[second_index]].y);
            vbo_data.push_back(normals[normalIndices[second_index]].z);
            vbo_data.push_back(uvs[uvIndices[second_index]].x);
            vbo_data.push_back(uvs[uvIndices[second_index]].y);
            vbo_data.push_back(is_base_triangle == 1 ? 1.0 : -1.0f);

            indices.push_back(counter_index);
            pushed_vertices[{ vertexIndices[second_index],
                              normalIndices[second_index],
                              uvIndices[second_index], is_base_triangle }] = counter_index++;
        }
        else
        {
            indices.push_back(pushed_vertices[{ vertexIndices[second_index],
                                                normalIndices[second_index],
                                                uvIndices[second_index], is_base_triangle }]);
        }
        
        if (pushed_vertices.find({ right_vertex_index, right_normal_index,
                                   right_uv_index, is_base_triangle })
            == pushed_vertices.end())
        {
            vbo_data.push_back(vertices[right_vertex_index].x);
            vbo_data.push_back(vertices[right_vertex_index].y);
            vbo_data.push_back(vertices[right_vertex_index].z);
            vbo_data.push_back(normals[right_normal_index].x);
            vbo_data.push_back(normals[right_normal_index].y);
            vbo_data.push_back(normals[right_normal_index].z);
            vbo_data.push_back(uvs[right_uv_index].x);
            vbo_data.push_back(uvs[right_uv_index].y);
            vbo_data.push_back(is_base_triangle == 1 ? 1.0 : -1.0f);

            indices.push_back(counter_index);
            pushed_vertices[{ right_vertex_index, right_normal_index,
                              right_uv_index, is_base_triangle }] = counter_index++;
        }
        else
        {
            indices.push_back(
                pushed_vertices[{ right_vertex_index, right_normal_index,
                                  right_uv_index, is_base_triangle }]);
        }

        if (pushed_vertices.find({ left_vertex_index, left_normal_index,
                                   left_uv_index, is_base_triangle })
                                   //left_uv_index, 0 })
            == pushed_vertices.end())
        {
            vbo_data.push_back(vertices[left_vertex_index].x);
            vbo_data.push_back(vertices[left_vertex_index].y);
            vbo_data.push_back(vertices[left_vertex_index].z);
            vbo_data.push_back(normals[left_normal_index].x);
            vbo_data.push_back(normals[left_normal_index].y);
            vbo_data.push_back(normals[left_normal_index].z);
            vbo_data.push_back(uvs[left_uv_index].x);
            vbo_data.push_back(uvs[left_uv_index].y);
            vbo_data.push_back(is_base_triangle == 1 ? 1.0 : -1.0f);
            //vbo_data.push_back(-1.0);

            indices.push_back(counter_index);
            pushed_vertices[{ left_vertex_index, left_normal_index,
                              left_uv_index, is_base_triangle }] = counter_index++;
                              //left_uv_index, 0 }] = counter_index++;
        }
        else
        {
            indices.push_back(
                pushed_vertices[{ left_vertex_index, left_normal_index,
                                  left_uv_index, is_base_triangle }]);
                                  //left_uv_index, 0 }]);
        }
    }
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
