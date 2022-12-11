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

/*void load_obj(const char *filename, std::vector<glm::vec3> &vertices, std::vector<unsigned int> &indices,
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
            vertices.push_back(v);
        }
        else if (line.substr(0, 2) == "vt")
        {
            std::istringstream s(line.substr(2));
            glm::vec2 v;
            s >> v.x;
            s >> v.y;
            uvs.push_back(v);
        }
        else if (line.substr(0, 2) == "vn")
        {
            std::istringstream s(line.substr(2));
            glm::vec3 v;
            s >> v.x;
            s >> v.y;
            s >> v.z;
            normals.push_back(v);
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
    }
    
    //  Build a hash table that maps from a half-edge (expressed as a pair of indices, in order) to the third index of the triangle the half-edge belongs to.
    std::map<std::pair<unsigned int, unsigned int>, unsigned int> halfEdgeToVertex;
    std::map<std::pair<unsigned int, unsigned int>, unsigned int> halfEdgeToNormal;
    std::map<std::pair<unsigned int, unsigned int>, unsigned int> halfEdgeToUv;
    
    for (unsigned int i = 0; i < vertexIndices.size(); i += 3)
    {
        halfEdgeToVertex[std::make_pair(vertexIndices[i], vertexIndices[i + 1])] = vertexIndices[i + 2];
        halfEdgeToNormal[std::make_pair(vertexIndices[i], vertexIndices[i + 1])] = normalIndices[i + 2];
        halfEdgeToUv[std::make_pair(vertexIndices[i], vertexIndices[i + 1])] = uvIndices[i + 2];
        
        halfEdgeToVertex[std::make_pair(vertexIndices[i + 1], vertexIndices[i + 2])] = vertexIndices[i];
        halfEdgeToNormal[std::make_pair(vertexIndices[i + 1], vertexIndices[i + 2])] = normalIndices[i];
        halfEdgeToUv[std::make_pair(vertexIndices[i + 1], vertexIndices[i + 2])] = uvIndices[i];
        
        halfEdgeToVertex[std::make_pair(vertexIndices[i + 2], vertexIndices[i])] = vertexIndices[i + 1];
        halfEdgeToNormal[std::make_pair(vertexIndices[i + 2], vertexIndices[i])] = normalIndices[i + 1];
        halfEdgeToUv[std::make_pair(vertexIndices[i + 2], vertexIndices[i])] = uvIndices[i + 1];
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
            std::pair<unsigned int, unsigned int> otherHalfEdge = std::make_pair(vertexIndex[(j+1)%3], vertexIndex[j]);

            unsigned int adjacentVertex_index = halfEdgeToVertex[otherHalfEdge];
            unsigned int adjacentNormal_index = halfEdgeToNormal[otherHalfEdge];
            unsigned int adjacentUv_index = halfEdgeToUv[otherHalfEdge];
            indices.push_back(vertexIndex[j]);
            indices.push_back(normalIndex[j]);
            indices.push_back(uvIndex[j]);

            indices.push_back(adjacentVertex_index);
            indices.push_back(adjacentNormal_index);
            indices.push_back(adjacentUv_index);
        }
    }
}*/

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
    
    std::map<std::array<unsigned int, 3>, unsigned int> pushed_vertices;

    int counter_index = 0;
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
            std::pair<unsigned int, unsigned int> otherHalfEdge = std::make_pair(vertexIndex[(j+1)%3], vertexIndex[j]);

            unsigned int adjacentVertex_index = halfEdgeToVertex[otherHalfEdge];
            unsigned int adjacentNormal_index = halfEdgeToNormal[otherHalfEdge];
            unsigned int adjacentUv_index = halfEdgeToUv[otherHalfEdge];
            /*vbo_data.push_back(vertices[vertexIndex[j]].x);
            vbo_data.push_back(vertices[vertexIndex[j]].y);
            vbo_data.push_back(vertices[vertexIndex[j]].z);
            vbo_data.push_back(normals[normalIndex[j]].x);
            vbo_data.push_back(normals[normalIndex[j]].y);
            vbo_data.push_back(normals[normalIndex[j]].z);
            vbo_data.push_back(uvs[uvIndex[j]].x);
            vbo_data.push_back(uvs[uvIndex[j]].y);

            indices.push_back(counter_index++);

            vbo_data.push_back(vertices[vertexIndex[(j + 1) % 3]].x);
            vbo_data.push_back(vertices[vertexIndex[(j + 1) % 3]].y);
            vbo_data.push_back(vertices[vertexIndex[(j + 1) % 3]].z);
            vbo_data.push_back(normals[normalIndex[(j + 1) % 3]].x);
            vbo_data.push_back(normals[normalIndex[(j + 1) % 3]].y);
            vbo_data.push_back(normals[normalIndex[(j + 1) % 3]].z);
            vbo_data.push_back(uvs[uvIndex[(j + 1) % 3]].x);
            vbo_data.push_back(uvs[uvIndex[(j + 1) % 3]].y);

            indices.push_back(counter_index++);
            
            vbo_data.push_back(vertices[adjacentVertex_index].x);
            vbo_data.push_back(vertices[adjacentVertex_index].y);
            vbo_data.push_back(vertices[adjacentVertex_index].z);
            vbo_data.push_back(normals[adjacentNormal_index].x);
            vbo_data.push_back(normals[adjacentNormal_index].y);
            vbo_data.push_back(normals[adjacentNormal_index].z);
            vbo_data.push_back(uvs[adjacentUv_index].x);
            vbo_data.push_back(uvs[adjacentUv_index].y);

            indices.push_back(counter_index++);

            vbo_data.push_back(vertices[vertexIndex[(j + 2) % 3]].x);
            vbo_data.push_back(vertices[vertexIndex[(j + 2) % 3]].y);
            vbo_data.push_back(vertices[vertexIndex[(j + 2) % 3]].z);
            vbo_data.push_back(normals[normalIndex[(j + 2) % 3]].x);
            vbo_data.push_back(normals[normalIndex[(j + 2) % 3]].y);
            vbo_data.push_back(normals[normalIndex[(j + 2) % 3]].z);
            vbo_data.push_back(uvs[uvIndex[(j + 2) % 3]].x);
            vbo_data.push_back(uvs[uvIndex[(j + 2) % 3]].y);

            indices.push_back(counter_index++);
            */
            
            if (pushed_vertices.find({vertexIndex[j], normalIndex[j], uvIndex[j]}) == pushed_vertices.end())
            {
                vbo_data.push_back(vertices[vertexIndex[j]].x);
                vbo_data.push_back(vertices[vertexIndex[j]].y);
                vbo_data.push_back(vertices[vertexIndex[j]].z);
                vbo_data.push_back(normals[normalIndex[j]].x);
                vbo_data.push_back(normals[normalIndex[j]].y);
                vbo_data.push_back(normals[normalIndex[j]].z);
                vbo_data.push_back(uvs[uvIndex[j]].x);
                vbo_data.push_back(uvs[uvIndex[j]].y);

                indices.push_back(counter_index);
                pushed_vertices[{vertexIndex[j], normalIndex[j], uvIndex[j]}] = counter_index++;
            }
            else
            {
                indices.push_back(pushed_vertices[{vertexIndex[j], normalIndex[j], uvIndex[j]}]);
            }
            
            if (pushed_vertices.find({vertexIndex[(j+1)%3], normalIndex[(j+1) % 3], uvIndex[(j + 1) % 3]}) == pushed_vertices.end())
            {
                vbo_data.push_back(vertices[vertexIndex[(j+1)%3]].x);
                vbo_data.push_back(vertices[vertexIndex[(j+1)%3]].y);
                vbo_data.push_back(vertices[vertexIndex[(j+1)%3]].z);
                vbo_data.push_back(normals[normalIndex[(j+1)%3]].x);
                vbo_data.push_back(normals[normalIndex[(j+1)%3]].y);
                vbo_data.push_back(normals[normalIndex[(j+1)%3]].z);
                vbo_data.push_back(uvs[uvIndex[(j+1)%3]].x);
                vbo_data.push_back(uvs[uvIndex[(j+1)%3]].y);

                indices.push_back(counter_index);
                pushed_vertices[{vertexIndex[(j+1)%3], normalIndex[(j+1) % 3], uvIndex[(j + 1) % 3]}] = counter_index++;
            }
            else
            {
                indices.push_back(pushed_vertices[{vertexIndex[(j+1)%3], normalIndex[(j+1) % 3], uvIndex[(j + 1) % 3]}]);
            }
            
            if (pushed_vertices.find({adjacentVertex_index, adjacentNormal_index, adjacentUv_index}) == pushed_vertices.end())
            {
                vbo_data.push_back(vertices[adjacentVertex_index].x);
                vbo_data.push_back(vertices[adjacentVertex_index].y);
                vbo_data.push_back(vertices[adjacentVertex_index].z);
                vbo_data.push_back(normals[adjacentNormal_index].x);
                vbo_data.push_back(normals[adjacentNormal_index].y);
                vbo_data.push_back(normals[adjacentNormal_index].z);
                vbo_data.push_back(uvs[adjacentUv_index].x);
                vbo_data.push_back(uvs[adjacentUv_index].y);

                indices.push_back(counter_index);
                pushed_vertices[{adjacentVertex_index, adjacentNormal_index, adjacentUv_index}] = counter_index++;
            }
            else
            {
                indices.push_back(pushed_vertices[{adjacentVertex_index, adjacentNormal_index, adjacentUv_index}]);
            }
            
            if (pushed_vertices.find({vertexIndex[(j+2)%3], normalIndex[(j+2) % 3], uvIndex[(j + 2) % 3]}) == pushed_vertices.end())
            {
                vbo_data.push_back(vertices[vertexIndex[(j+2)%3]].x);
                vbo_data.push_back(vertices[vertexIndex[(j+2)%3]].y);
                vbo_data.push_back(vertices[vertexIndex[(j+2)%3]].z);
                vbo_data.push_back(normals[normalIndex[(j+2)%3]].x);
                vbo_data.push_back(normals[normalIndex[(j+2)%3]].y);
                vbo_data.push_back(normals[normalIndex[(j+2)%3]].z);
                vbo_data.push_back(uvs[uvIndex[(j+2)%3]].x);
                vbo_data.push_back(uvs[uvIndex[(j+2)%3]].y);

                indices.push_back(counter_index);
                pushed_vertices[{vertexIndex[(j+2)%3], normalIndex[(j+2) % 3], uvIndex[(j + 2) % 3]}] = counter_index++;
            }
            else
            {
                indices.push_back(pushed_vertices[{vertexIndex[(j+2)%3], normalIndex[(j+2) % 3], uvIndex[(j + 2) % 3]}]);
            }
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
