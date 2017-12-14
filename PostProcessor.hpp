#ifndef POST_PROCESSOR_HPP
#define POST_PROCESSOR_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Texture.hpp"
#include "SpriteRenderer.hpp"
#include "Shader.hpp"


// PostProcessor hosts all PostProcessing effects for the Breakout
// Game. It renders the game on a textured quad after which one can
// enable specific effects by enabling either the Confuse, Chaos or
// Shake boolean.
// It is required to call BeginRender() before rendering the game
// and EndRender() after rendering the game for the class to work.

enum class Effect : short
{
    CONFUSE, SHAKE, CHAOS, NORMAL
};

class PostProcessor
{
public:
    // Constructor
    PostProcessor(const Shader & shader, GLuint width, GLuint height);
    ~PostProcessor();
    // Prepares the postprocessor's framebuffer operations before rendering the game
    void beginRender();
    // Should be called after rendering the game, so it stores all the rendered data into a texture object
    void endRender();
    // Renders the PostProcessor texture quad (as a screen-encompassing large sprite)
    void render(GLfloat time);
    // Enable effect
    void enable(Effect);
private:
    // Render state
    GLuint FBO; // FBO is regular, used for blitting MS color-buffer to texture
    GLuint RBO; // RBO is used for multisampled color buffer
    GLuint VAO;
    // State
    Shader postProcessingShader;
    Texture2D texture;
    // Options
    GLuint width, height;
    GLboolean confuse, shake, chaos;
    // Initialize quad for rendering postprocessing texture
    void initRenderData();
};

#endif