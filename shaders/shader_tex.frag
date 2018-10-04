#version 430 core
out vec4 FragColor;

uniform sampler2D textureSampler;
uniform vec3 lightDir;

in vec3 interpNormal;
in vec2 interpTexCoord;
in vec3 vertexWorldPosition;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main()
{
    float ratio = 1.00 / 1.52;

	vec3 normal = normalize(interpNormal);
	vec3 I = normalize(cameraPos - vertexWorldPosition);
	//vec3 R = refract(I, normalize(interpNormal), ratio);
    vec3 R = reflect(-I, normalize(interpNormal));
    FragColor = vec4(texture(skybox, R).rgb, 1.0);
}
