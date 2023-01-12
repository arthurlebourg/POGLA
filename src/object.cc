#include "object.hh"

Object::Object(const std::string obj_file, const std::string texture,
               const glm::vec3 position, const float obj_mass)
    : position_(position)
    , transform_(glm::mat4(1.0f))
    , mass_(obj_mass)
    , texture_(tifo::load_image(texture.c_str()))
    , mesh_triangles_(Mesh(3))
    , mesh_segments_(Mesh(4))
{
    load_obj(obj_file.c_str(), mesh_triangles_, mesh_segments_);
    // create a dynamic rigidbody

    btConvexHullShape *shape = new btConvexHullShape();

    //std::cout << "Vertices: " << mesh_.get_vertices().size() << std::endl;
    for (auto v : mesh_triangles_.get_vertices())
    {
        auto i = v.position;
        shape->addPoint(btVector3(i.x, i.y, i.z));
    }
    shape->optimizeConvexHull();
    colShape_ = shape;

    /// Create Dynamic Objects
    btTransform startTransform;
    startTransform.setIdentity();

    btScalar mass(obj_mass);

    // rigidbody is dynamic if and only if mass is non zero, otherwise static
    bool isDynamic = (mass != 0.f);

    btVector3 localInertia(0, 0, 0);
    if (isDynamic)
        colShape_->calculateLocalInertia(mass, localInertia);

    move(position);
    startTransform.setOrigin(btVector3(position.x, position.y, position.z));

    // using motionstate is recommended, it provides interpolation capabilities,
    // and only synchronizes 'active' objects
    btDefaultMotionState *myMotionState =
        new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState,
                                                    colShape_, localInertia);
    body_ = new btRigidBody(rbInfo);

    glGenTextures(1, &texture_id_);
    TEST_OPENGL_ERROR();
    //glActiveTexture(GL_TEXTURE0);
    TEST_OPENGL_ERROR();
    glBindTexture(GL_TEXTURE_2D, texture_id_);
    TEST_OPENGL_ERROR();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_->sx, texture_->sy, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, texture_->pixels);
    TEST_OPENGL_ERROR();

    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
};

unsigned int Object::get_texture()
{
    return texture_id_;
}

btRigidBody *Object::get_body()
{
    return body_;
}

glm::mat4 Object::move(const glm::vec3 pos)
{
    transform_ = glm::translate(transform_, pos - position_);
    position_ = pos - position_;
    return transform_;
}

void Object::add_force(const glm::vec3 force)
{
    body_->applyCentralForce(btVector3(force.x, force.y, force.z));
    
}

void Object::add_torque(const glm::vec3 torque)
{
    body_->applyTorque(btVector3(torque.x, torque.y, torque.z));
}

glm::mat4 Object::get_transform()
{
    return transform_;
}

void Object::set_transform(btScalar *mat)
{
    transform_ = glm::make_mat4(mat);
}

glm::vec3 Object::get_position()
{
    return position_;
}

btCollisionShape *Object::get_colShape()
{
    return colShape_;
}

void Object::draw_triangles(const Shader &shader)
{
    //shader.use();
    glActiveTexture(GL_TEXTURE0);TEST_OPENGL_ERROR();
    glBindTexture(GL_TEXTURE_2D, texture_id_);TEST_OPENGL_ERROR();
    unsigned tex_location =
        glGetUniformLocation(shader.shader_program_, "texture_sampler");TEST_OPENGL_ERROR();
    glUniform1i(tex_location, 0);
    TEST_OPENGL_ERROR();
    shader.set_mat4_uniform("transform", transform_);
    TEST_OPENGL_ERROR();
    mesh_triangles_.draw();
    TEST_OPENGL_ERROR();
}

void Object::draw_segments(const Shader &shader)
{
    //shader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id_);
    unsigned tex_location =
        glGetUniformLocation(shader.shader_program_, "texture_sampler");
    glUniform1i(tex_location, 0);
    TEST_OPENGL_ERROR();
    shader.set_mat4_uniform("transform", transform_);
    TEST_OPENGL_ERROR();
    mesh_segments_.draw();
    TEST_OPENGL_ERROR();
}