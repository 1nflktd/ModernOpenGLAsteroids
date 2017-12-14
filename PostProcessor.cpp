#include "PostProcessor.hpp"

#include <iostream>

PostProcessor::PostProcessor(const Shader & shader, GLuint width, GLuint height)
    : postProcessingShader(shader), texture(), width(width), height(height), confuse(GL_FALSE), shake(GL_FALSE), chaos(GL_FALSE)
{
    // Initialize renderbuffer/framebuffer object
    glGenFramebuffers(1, &this->FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
    glGenRenderbuffers(1, &this->RBO);

    // Initialize renderbuffer storage with a multisampled color buffer (don't need a depth/stencil buffer)
    glBindRenderbuffer(GL_RENDERBUFFER, this->RBO);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); // Use a single renderbuffer object for both a depth AND stencil buffer.
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->RBO); // Now actually attach it

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::POSTPROCESSOR: Failed to initialize FBO" << std::endl;

    this->texture.generate(width, height, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->texture.ID, 0); // Attach texture to framebuffer as its color attachment
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::POSTPROCESSOR: Failed to initialize FBO" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Initialize render data and uniforms
    this->initRenderData();
    this->postProcessingShader.SetInteger("scene", 0, GL_TRUE);

    GLfloat offset = 1.0f / 300.0f; // 300.0f
    GLfloat offsets[9][2] = {
        { -offset,  offset  },  // top-left
        {  0.0f,    offset  },  // top-center
        {  offset,  offset  },  // top-right
        { -offset,  0.0f    },  // center-left
        {  0.0f,    0.0f    },  // center-center
        {  offset,  0.0f    },  // center - right
        { -offset, -offset  },  // bottom-left
        {  0.0f,   -offset  },  // bottom-center
        {  offset, -offset  }   // bottom-right
    };
    glUniform2fv(glGetUniformLocation(this->postProcessingShader.ID, "offsets"), 9, (GLfloat*)offsets);

    GLint edgeKernel[9] = {
        -1, -1, -1,
        -1,  8, -1,
        -1, -1, -1
    };
    glUniform1iv(glGetUniformLocation(this->postProcessingShader.ID, "edgeKernel"), 9, edgeKernel);

    GLfloat blurKernel[9] = {
        1.0 / 16, 2.0 / 16, 1.0 / 16,
        2.0 / 16, 4.0 / 16, 2.0 / 16,
        1.0 / 16, 2.0 / 16, 1.0 / 16
    };
    glUniform1fv(glGetUniformLocation(this->postProcessingShader.ID, "blurKernel"), 9, blurKernel);
}

PostProcessor::~PostProcessor()
{
    glDeleteFramebuffers(1, &this->FBO);
}

void PostProcessor::beginRender()
{
    glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // EU
    glEnable(GL_DEPTH_TEST);
}

void PostProcessor::endRender()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // EU
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT/* | GL_DEPTH_BUFFER_BIT*/);
    glDisable(GL_DEPTH_TEST);
}

void PostProcessor::render(GLfloat time)
{
    // Set uniforms/options
    this->postProcessingShader.Use();
    this->postProcessingShader.SetFloat("time", time);
    this->postProcessingShader.SetInteger("confuse", this->confuse);
    this->postProcessingShader.SetInteger("shake", this->shake);
    this->postProcessingShader.SetInteger("chaos", this->chaos);
    // Render textured quad
    glActiveTexture(GL_TEXTURE0);
    this->texture.bind();
    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void PostProcessor::initRenderData()
{
    // Configure VAO/VBO
    GLuint VBO;
    GLfloat vertices[] = {
        // Pos        // Tex
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 1.0f,

        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f
    };
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(this->VAO);

    GLint atrib_vertex = glGetAttribLocation(this->postProcessingShader.ID, "vertex");
    glVertexAttribPointer(atrib_vertex, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (GLvoid*)0);
    glEnableVertexAttribArray(atrib_vertex);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void PostProcessor::enable(Effect effect)
{
    this->confuse = GL_FALSE;
    this->shake = GL_FALSE;
    this->chaos = GL_FALSE;
    switch (effect)
    {
        case Effect::CONFUSE:
            this->confuse = GL_TRUE;
            break;
        case Effect::SHAKE:
            this->shake = GL_TRUE;
            break;
        case Effect::CHAOS:
            this->chaos = GL_TRUE;
            break;
        default: // NORMAL
            break;
    }
}
