// #version 140
#version 330 core

in vec4 vertex; // <vec2 position, vec2 texCoords>
out vec2 TexCoords;

uniform bool chaos;
uniform bool confuse;
uniform bool shake;
uniform float time;

void main()
{
    gl_Position = vec4(vertex.xy, 0.0f, 1.0f);
    vec2 texture = vertex.zw;

    if(chaos)
    {
        vec2 pos = vec2(texture.x + sin(time), texture.y + cos(time));
        TexCoords = pos;
    }
    else if (confuse)
    {
        TexCoords = vec2(1.0 - texture.x, 1.0 - texture.y);
    }
    else
    {
        TexCoords = texture;
    }

    if (shake)
    {
        float strength = 0.01;
        gl_Position.x += cos(time * 1000) * strength;
        gl_Position.y += cos(time * 1500) * strength;
    }
}
