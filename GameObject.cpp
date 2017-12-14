#include "GameObject.hpp"

GameObject::GameObject()
    : position(0, 0, 0), size(1, 1, 1), velocity(0.0f), color(1.0f), rotation(0.0f), destroyed(false), sprite() { }

GameObject::GameObject(const glm::vec3 & pos, const glm::vec3 & size, const glm::vec3 & color, const glm::vec3 & velocity, GLfloat rotation, const Texture2D & sprite)
    : position(pos), size(size), velocity(velocity), color(color), rotation(rotation), destroyed(false), sprite(sprite) { }

void GameObject::draw(SpriteRenderer &renderer)
{
    renderer.drawSprite(this->sprite, this->position, this->size, this->rotation, this->color);
}
