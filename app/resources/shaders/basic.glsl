//#shader vertex
#version  330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

void main() {
    FragPos = vec3(M * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(M))) * aNormal;
    TexCoords = vec2(aTexCoords.x, 1.0 - aTexCoords.y);

    gl_Position = P * V * M * vec4(aPos, 1.0);
}

//#shader fragment

#version  330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D DiffuseTexture;

//uniform vec3 LightPos;
//uniform vec3 ViewPos;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shi;
};

struct DirLight {
    vec3 dir;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 pos;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    vec3 clq;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform vec3 LightPos;
uniform vec3 ViewPos;
uniform DirLight dirLight;
uniform SpotLight spotLight;
uniform Material material;
uniform bool blin;

uniform vec3 emissive;
uniform float emissiveStrength;

vec3 DirLightCalculation(DirLight light, vec3 normal, vec3 viewDir) {
    vec3 L = normalize(-light.dir);
    float diff = max(dot(normal, L), 0.0f);
    vec3 R = reflect(-L, normal);
    float spec = 0.0f;


    if (blin) {
        vec3 H = normalize(L + viewDir);
        spec = pow(max(dot(viewDir, H), 0.0f), material.shi);
    } else {
        spec = pow(max(dot(viewDir, R), 0.0f), material.shi);
    }

    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;
    vec3 specular = spec * light.specular;

    return ambient + diffuse + specular;
}

vec3 SpotLightCalculation(SpotLight light, vec3 normal, vec3 viewDir,  vec3 fragPos) {
    vec3 L = normalize(light.pos - fragPos);
    float diff = max(dot(normal, L), 0.0f);
    vec3 R = reflect(-L, normal);
    float spec = 0.0f;

    if (blin) {
        vec3 H = normalize(L + viewDir);
        spec = pow(max(dot(H, normal), 0.0f), material.shi);
    } else {
        spec = pow(max(dot(viewDir, R), 0.0f), material.shi);
    }

    float d = length(light.pos - fragPos);
    float att = 1.0f / (light.clq.x + light.clq.y * d + light.clq.z * d * d);

    float theta = dot(L, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff)/epsilon, 0.0f, 1.0f);

    vec3 ambient = intensity * att * light.ambient * texture(material.diffuse, TexCoords).rgb;
    vec3 diffuse = intensity * att * light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;
    vec3 specular = intensity * att * light.specular * spec * texture(material.specular, TexCoords).rgb;

    return ambient + diffuse + specular;
}

void main() {

    vec3 N = normalize(Normal);
    vec3 V = normalize(ViewPos - FragPos);

    vec3 result = DirLightCalculation(dirLight, N, V);
    result += SpotLightCalculation(spotLight, N, V, FragPos);

    vec3 color = result + result * emissive * emissiveStrength;

    FragColor = vec4(result, 1.0);

    float brightness = dot(color, vec3(0.2126f, 0.7152f, 0.0722f));

    if (brightness > 1.0) {
        BrightColor = vec4(color, 1.0f);
    } else {
        BrightColor = vec4(0.0f);
    }
}