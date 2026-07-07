//#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}

//#shader fragment
#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skyboxTex;

void main()
{
    FragColor = texture(skyboxTex, TexCoords);
}