#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Texture.hpp"
#include "Shader.hpp"

class SpriteRenderer
{
public:
    // Constructor (inits shaders/shapes)
    SpriteRenderer(const Shader & _shader);
    // Destructor
    ~SpriteRenderer();
    // Renders a defined quad textured with given sprite
    void drawSprite(const Texture2D & texture, const glm::vec3 & position, const glm::vec3 & size, GLfloat rotate, const glm::vec3 & color);
private:
    // Render state
    Shader shader;
    GLuint quadVAO;
    // Initializes and configures the quad's buffer and vertex attributes
    void initRenderData();
};

#endif