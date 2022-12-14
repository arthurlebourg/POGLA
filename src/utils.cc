#include "utils.hh"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

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
    exit(1);
}

void load_obj(const char *filename, Mesh &triangle_mesh, Mesh &segments_mesh)
{
    tinyobj::ObjReaderConfig reader_config;
    reader_config.mtl_search_path = "./objects/"; // Path to material files

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(filename, reader_config))
    {
        if (!reader.Error().empty())
        {
            std::cerr << "TinyObjReader: " << reader.Error();
        }
        exit(1);
    }

    if (!reader.Warning().empty())
    {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices_triangles;
    std::vector<unsigned int> indices_segments;

    std::map<Vertex, unsigned int> pushed_vertices;
    std::set<std::pair<Vertex, Vertex>> pushed_edges;

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& materials = reader.GetMaterials();

    std::map<std::pair<Vertex, Vertex>, size_t> half_edge_to_vertex_index;
    
    int counter_index = 0;
    // Loop over shapes
    for (size_t s = 0; s < shapes.size(); s++)
    {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
        {
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
            if (fv != 3)
            {
                std::cerr << "Error: face with " << fv << " vertices. please use triangulated faces" << std::endl;
                exit(1);
            }
            std::array<size_t, 3> face_vertices;

            // Optional: vertex colors
            tinyobj::real_t red = 1.0;
            tinyobj::real_t green = 1.0;
            tinyobj::real_t blue = 1.0;
            // per-face material
            if (shapes[s].mesh.material_ids.size() > 0 && materials.size() > 0)
            {
                size_t mv = shapes[s].mesh.material_ids[f]; // https://fr.wikipedia.org/wiki/Material_Template_Library
                auto mat = materials[mv];
                //red = mat.ambient[0];
                //green = mat.ambient[1];
                //blue = mat.ambient[2];
                red = mat.diffuse[0];
                green = mat.diffuse[1];
                blue = mat.diffuse[2];
                
                /*std::cout << std::endl << "material " << mv << " " << mat.name
                << std::endl; std::cout << "ambient " << mat.ambient[0] << " "
                << mat.ambient[1] << " " << mat.ambient[2] << std::endl;
                std::cout << "diffuse " << mat.diffuse[0] << " " <<
                mat.diffuse[1] << " " << mat.diffuse[2] << std::endl; std::cout
                << "specular " << mat.specular[0] << " " << mat.specular[1] << "
                " << mat.specular[2] << std::endl; std::cout << "transmittance "
                << mat.transmittance[0] << " " << mat.transmittance[1] << " " <<
                mat.transmittance[2] << std::endl; std::cout << "emission " <<
                mat.emission[0] << " " << mat.emission[1] << " " <<
                mat.emission[2] << std::endl; std::cout << "shininess " <<
                mat.shininess << std::endl; std::cout << "ior " << mat.ior <<
                std::endl; std::cout << "dissolve " << mat.dissolve <<
                std::endl; std::cout << "illum " << mat.illum << std::endl;
                std::cout << "ambient_texname " << mat.ambient_texname <<
                std::endl; std::cout << "diffuse_texname " <<
                mat.diffuse_texname << std::endl; std::cout << "specular_texname
                " << mat.specular_texname << std::endl; std::cout <<
                "specular_highlight_texname " << mat.specular_highlight_texname
                << std::endl; std::cout << "bump_texname " << mat.bump_texname
                << std::endl; std::cout << "displacement_texname " <<
                mat.displacement_texname << std::endl; std::cout <<
                "alpha_texname " << mat.alpha_texname << std::endl; std::cout <<
                "roughness_texname " << mat.roughness_texname << std::endl;
                std::cout << "metallic_texname " << mat.metallic_texname <<
                std::endl; std::cout << "sheen_texname " << mat.sheen_texname <<
                std::endl; std::cout << "emissive_texname " <<
                mat.emissive_texname << std::endl; std::cout << "normal_texname
                " << mat.normal_texname << std::endl; std::cout <<
                "unknown_parameter " << mat.unknown_parameter.size() <<
                std::endl;*/
            }
            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++)
            {
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                tinyobj::real_t vx =
                    attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                tinyobj::real_t vy =
                    attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                tinyobj::real_t vz =
                    attrib.vertices[3 * size_t(idx.vertex_index) + 2];

                // Check if `normal_index` is zero or positive. negative = no
                // normal data

                tinyobj::real_t nx = 0.0;
                tinyobj::real_t ny = 0.0;
                tinyobj::real_t nz = 0.0;
                if (idx.normal_index >= 0)
                {
                    nx =
                        attrib.normals[3 * size_t(idx.normal_index) + 0];
                    ny =
                        attrib.normals[3 * size_t(idx.normal_index) + 1];
                    nz =
                        attrib.normals[3 * size_t(idx.normal_index) + 2];   
                }

                // Check if `texcoord_index` is zero or positive. negative = no
                // texcoord data

                tinyobj::real_t tx = 0.0;
                tinyobj::real_t ty = 0.0;
                if (idx.texcoord_index >= 0)
                {
                    tx =
                        attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
                    ty =
                        attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
                }

                /*if (attrib.colors.size() > 0)
                {
                    red =
                        attrib.colors[3 * size_t(idx.vertex_index) + 0];
                    green =
                        attrib.colors[3 * size_t(idx.vertex_index) + 1];
                    blue =
                        attrib.colors[3 * size_t(idx.vertex_index) + 2];
                }*/
                
                Vertex vertex(glm::vec3(vx, vy, vz), glm::vec3(nx, ny, nz), glm::vec3(red, green, blue), glm::vec2(tx, ty));
                if (pushed_vertices.find(vertex) != pushed_vertices.end())
                {
                    indices_triangles.push_back(pushed_vertices[vertex]);
                    face_vertices[v] = pushed_vertices[vertex];
                }
                else
                {
                    vertices.push_back(vertex);
                    indices_triangles.push_back(counter_index);
                    face_vertices[v] = counter_index;
                    pushed_vertices[vertex] = counter_index++;
                }
            }
            index_offset += fv;
            half_edge_to_vertex_index[std::make_pair(vertices[face_vertices[0]], vertices[face_vertices[1]])] = face_vertices[2];
            half_edge_to_vertex_index[std::make_pair(vertices[face_vertices[1]], vertices[face_vertices[2]])] = face_vertices[0];
            half_edge_to_vertex_index[std::make_pair(vertices[face_vertices[2]], vertices[face_vertices[0]])] = face_vertices[1];

        }
    }

    triangle_mesh.load_mesh(vertices, indices_triangles);
    
    for (size_t i = 0; i < indices_triangles.size(); i++)
    {
        size_t next_index = i % 3 == 2 ? i - 2 : i + 1;
        Vertex v1 = vertices[indices_triangles[i]];
        Vertex v2 = vertices[indices_triangles[next_index]];
        if (pushed_edges.find(std::make_pair(v1, v2)) != pushed_edges.end())
        {
            continue;
        }
        pushed_edges.insert(std::make_pair(v1, v2));
        pushed_edges.insert(std::make_pair(v2, v1));

        std::pair<Vertex, Vertex> left_edge =
            std::make_pair(v2, v1); // left is the adjacent vertex

        std::pair<Vertex, Vertex> right_edge =
            std::make_pair(v1, v2);

        size_t left_vertex = half_edge_to_vertex_index[left_edge];
        size_t right_vertex = half_edge_to_vertex_index[right_edge];

        indices_segments.push_back(indices_triangles[i]);
        indices_segments.push_back(indices_triangles[next_index]);
        indices_segments.push_back(right_vertex);
        indices_segments.push_back(left_vertex);
    }
    segments_mesh.load_mesh(vertices, indices_segments);
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
