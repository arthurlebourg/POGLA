#include "player.hh"

Player::Player(const glm::vec3 position, const glm::vec3 direction)
    : yaw_(90.0)
    , pitch_(0.0)
    , position_(position)
    , direction_(direction)
    , up_(glm::vec3(0, 1, 0))
    , projection_(glm::frustum(-0.01, 0.01, -0.01, 0.01, 0.01, 300.0))
    , speed_(800.0f)
    , walk_(speed_)
    , sprint_(2.5 * speed_)
    , colShape_(new btBoxShape(btVector3(1.0, 5.0, 1.0)))
{
    /// Create Dynamic Objects
    btTransform startTransform;
    startTransform.setIdentity();

    btScalar mass(1.0);

    // rigidbody is dynamic if and only if mass is non zero, otherwise static
    bool isDynamic = (mass != 0.f);

    btVector3 localInertia(0, 0, 0);
    if (isDynamic)
        colShape_->calculateLocalInertia(mass, localInertia);

    startTransform.setOrigin(btVector3(position.x, position.y, position.z));

    // using motionstate is recommended, it provides interpolation capabilities,
    // and only synchronizes 'active' objects
    btDefaultMotionState *myMotionState =
        new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState,
                                                    colShape_, localInertia);
    body_ = new btRigidBody(rbInfo);
    body_->setAngularFactor(btVector3(0, 1, 0));
}

float Player::get_yaw()
{
    return yaw_;
}
void Player::add_yaw(const float f)
{
    yaw_ += f;
    yaw_ = reset_angle(yaw_, 360.0f);
}

void Player::set_yaw(const float f)
{
    yaw_ = f;
    yaw_ = reset_angle(yaw_, 360.0f);
}

float Player::get_pitch()
{
    return pitch_;
}

void Player::add_pitch(const float f)
{
    pitch_ += f;

    pitch_ = glm::clamp(pitch_, -89.0f, 89.0f);
}

void Player::set_pitch(const float f)
{
    pitch_ = f;

    pitch_ = glm::clamp(pitch_, -89.0f, 89.0f);
}

glm::vec3 Player::get_position()
{
    return position_;
}

void Player::set_speed(bool sprint)
{
    speed_ = sprint ? sprint_ : walk_;
}

void Player::set_position(float x, float y, float z)
{
    position_.x = x;
    position_.y = y;
    position_.z = z;
}

glm::vec3 Player::get_direction()
{
    return direction_;
}
void Player::set_direction(glm::vec3 dir)
{
    direction_ = dir;
}

void Player::normalize_direction()
{
    direction_ = glm::normalize(direction_);
}

glm::mat4 Player::get_model_view()
{
    return glm::lookAt(position_, position_ + direction_, up_);
}

glm::mat4 Player::get_projection()
{
    return projection_;
}

glm::vec3 Player::get_up()
{
    return up_;
}

btRigidBody *Player::get_body()
{
    return body_;
}

void Player::move(const int forward, const int sideward, const float deltaTime)
{
    body_->activate();
    btVector3 vel = body_->getLinearVelocity();

    //get right vector from model view matrix
    glm::vec3 right = glm::normalize(glm::cross(direction_, up_));
    //get forward vector from model view matrix
    glm::vec3 forwardvec = glm::normalize(glm::cross(up_, right));
    
    glm::vec3 dir = speed_ * deltaTime * ((float)forward * forwardvec + (float)sideward * right);
    vel.setX(dir.x);
    vel.setZ(dir.z);
    body_->setLinearVelocity(vel);
}
