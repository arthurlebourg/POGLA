#include "object.hh"

Object::Object(const std::string obj_file, const std::string texture,
               const glm::vec3 position, const float obj_mass)
    : position_(position)
    , transform_(glm::mat4(1.0f))
    , mass_(obj_mass)
    , texture_(tifo::load_image(texture.c_str()))
{
    unsigned int VBO;
    std::vector<float> vbo_data;
    load_obj(obj_file.c_str(), vertices_, uv_, normals_, indices_, vbo_data);
    vertices_number_ = vertices_.size();
    indices_number_ = indices_.size();

    /*int n = 0;

    for (unsigned int i = 0; i < indices_.size(); i++)
    {
        if (n % 3 == 0 && i % 4 == 0)
            std::cout << n << " // ";
        for (int j = 0; j < 9; j++)
        {
            std::cout << vbo_data[indices_[i] * 9 + j] << " ";
            if (j == 2 || j == 5 || j == 7)
                std::cout << "| ";
        }
        if (i % 4 == 2)
        {
            std::cout << "   ---";
        }
        if (i % 4 == 3)
        {
            if (n % 3 == 2)
                std::cout << std::endl;                
            std::cout << std::endl;
            n++;
        }
        else
        {
            std::cout << "   ";
        }
    }
    std::cout << std::endl;*/

    
    
    glGenBuffers(1, &VBO);
    TEST_OPENGL_ERROR();
    glGenBuffers(1, &EBO_);
    TEST_OPENGL_ERROR();
    glGenVertexArrays(1, &VAO_);
    TEST_OPENGL_ERROR();

    glBindVertexArray(VAO_);
    TEST_OPENGL_ERROR();
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    TEST_OPENGL_ERROR();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
    TEST_OPENGL_ERROR();

    glBufferData(GL_ARRAY_BUFFER, vbo_data.size() * sizeof(float), &vbo_data[0], GL_STATIC_DRAW);
    TEST_OPENGL_ERROR();

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), &indices_[0], GL_STATIC_DRAW);
    TEST_OPENGL_ERROR();

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float),
                          (void *)0);TEST_OPENGL_ERROR();
    glEnableVertexAttribArray(0);TEST_OPENGL_ERROR();

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);TEST_OPENGL_ERROR();

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float),
                          (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);TEST_OPENGL_ERROR();

    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float),
                          (void *)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);TEST_OPENGL_ERROR();

    /*glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float),
                          (void *)(9 * sizeof(float)));
    glEnableVertexAttribArray(4);TEST_OPENGL_ERROR();*/

    // create a dynamic rigidbody

    btConvexHullShape *shape = new btConvexHullShape();
    //int count = 0;
    for (auto i : vertices_)
    {
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

    GLint texture_units, combined_texture_units;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texture_units);
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &combined_texture_units);

    glGenTextures(1, &texture_id_);
    TEST_OPENGL_ERROR();
    glActiveTexture(GL_TEXTURE0);
    TEST_OPENGL_ERROR();
    glBindTexture(GL_TEXTURE_2D, texture_id_);
    TEST_OPENGL_ERROR();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_->sx, texture_->sy, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, texture_->pixels);
    TEST_OPENGL_ERROR();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    TEST_OPENGL_ERROR();
};


unsigned int Object::get_VAO()
{
    return VAO_;
}

unsigned int Object::get_vertices_number()
{
    return vertices_number_;
}

unsigned int Object::get_indices_number()
{
    return indices_number_;
}

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
