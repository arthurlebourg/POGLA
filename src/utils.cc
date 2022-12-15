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

    //std::map<Vertex, unsigned int> pushed_vertices;
    std::map<std::array<float, 11>, unsigned int> pushed_vertices;
    
    std::set<std::pair<unsigned int, unsigned int>> pushed_edges;

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& materials = reader.GetMaterials();

    //std::map<std::pair<unsigned int, unsigned int>, unsigned int> half_edge_to_vertex_index;
    std::vector<std::array<unsigned int, 3>> faces;
    
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
            std::array<unsigned int, 3> face_vertices;

            // Optional: vertex colors
            tinyobj::real_t red = 1.0;
            tinyobj::real_t green = 1.0;
            tinyobj::real_t blue = 1.0;
            // per-face material
            if (shapes[s].mesh.material_ids.size() > 0 && materials.size() > 0)
            {
                size_t mv = shapes[s].mesh.material_ids[f]; // https://fr.wikipedia.org/wiki/Material_Template_Library
                auto mat = materials[mv];
                red = mat.diffuse[0];
                green = mat.diffuse[1];
                blue = mat.diffuse[2];
                
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

                Vertex vertex = {glm::vec3(vx, vy, vz), glm::vec3(nx, ny, nz),
                                 glm::vec2(tx, ty), glm::vec3(red, green, blue)};
                //Vertex vertex(glm::vec3(vx, vy, vz), glm::vec3(nx, ny, nz), glm::vec3(red, green, blue), glm::vec2(tx, ty));
                if (pushed_vertices.find({vx, vy, vz, nx, ny, nz, tx, ty, red, green, blue}) != pushed_vertices.end())
                {
                    indices_triangles.push_back(pushed_vertices[{vx, vy, vz, nx, ny, nz, tx, ty, red, green, blue}]);
                    face_vertices[v] = pushed_vertices[{vx, vy, vz, nx, ny, nz, tx, ty, red, green, blue}];
                }
                else
                {
                    vertices.push_back(vertex);
                    indices_triangles.push_back(counter_index);
                    face_vertices[v] = counter_index;
                    pushed_vertices[{vx, vy, vz, nx, ny, nz, tx, ty, red, green, blue}] = counter_index++;
                }
            }
            index_offset += fv;
            faces.push_back(face_vertices);
            /*half_edge_to_vertex_index[std::make_pair(face_vertices[0], face_vertices[1])] = face_vertices[2];
            half_edge_to_vertex_index[std::make_pair(face_vertices[1], face_vertices[2])] = face_vertices[0];
            half_edge_to_vertex_index[std::make_pair(face_vertices[2], face_vertices[0])] = face_vertices[1];*/

            //std::cout << "face " << f << " : " << face_vertices[0] << "(" << vertices[face_vertices[0]].position.x << ", " << vertices[face_vertices[0]].position.y << ", " << vertices[face_vertices[0]].position.z << ") " << face_vertices[1] << "(" << vertices[face_vertices[1]].position.x << ", " << vertices[face_vertices[1]].position.y << ", " << vertices[face_vertices[1]].position.z << ") " << face_vertices[2] << "(" << vertices[face_vertices[2]].position.x << ", " << vertices[face_vertices[2]].position.y << ", " << vertices[face_vertices[2]].position.z << ") " << std::endl;


        }
    }

    triangle_mesh.load_mesh(vertices, indices_triangles);
    
    for (size_t i = 0; i < indices_triangles.size(); i++)
    {
        size_t next_index = i % 3 == 2 ? i - 2 : i + 1;
        unsigned int v1 = indices_triangles[i];
        unsigned int v2 = indices_triangles[next_index];
        if (pushed_edges.find(std::make_pair(v1, v2)) != pushed_edges.end())
        {
            continue;
        }
        pushed_edges.insert(std::make_pair(v1, v2));
        pushed_edges.insert(std::make_pair(v2, v1));

        /*std::pair<unsigned int, unsigned int> left_edge =
            std::make_pair(v2, v1); // left is the adjacent vertex

        std::pair<unsigned int, unsigned int> right_edge =
            std::make_pair(v1, v2);

        if (half_edge_to_vertex_index.find(left_edge) ==
            half_edge_to_vertex_index.end())
        {
            std::cout << "left edge not found" << std::endl;
        }
        if (half_edge_to_vertex_index.find(right_edge) ==
            half_edge_to_vertex_index.end())
        {
            std::cout << "right edge not found" << std::endl;
        }
        size_t left_vertex = half_edge_to_vertex_index[left_edge];
        size_t right_vertex = half_edge_to_vertex_index[right_edge];*/

        //find adjacent vertex in faces
        size_t left_vertex = 0;
        size_t right_vertex = 0;
        for (size_t f = 0; f < faces.size(); f++)
        {
            if (faces[f][0] == v1 && faces[f][1] == v2)
            {
                left_vertex = faces[f][2];
            }
            else if (faces[f][1] == v1 && faces[f][2] == v2)
            {
                left_vertex = faces[f][0];
            }
            else if (faces[f][2] == v1 && faces[f][0] == v2)
            {
                left_vertex = faces[f][1];
            }
            else if (faces[f][0] == v2 && faces[f][1] == v1)
            {
                right_vertex = faces[f][2];
            }
            else if (faces[f][1] == v2 && faces[f][2] == v1)
            {
                right_vertex = faces[f][0];
            }
            else if (faces[f][2] == v2 && faces[f][0] == v1)
            {
                right_vertex = faces[f][1];
            }
        }

        /*std::cout << "v1: " << v1 << std::endl;
        std::cout << "v2: " << v2 << std::endl;
        std::cout << "left vertex: " << left_vertex << std::endl;
        std::cout << "right vertex: " << right_vertex << std::endl << std::endl;*/


        indices_segments.push_back(v1);
        indices_segments.push_back(v2);
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
