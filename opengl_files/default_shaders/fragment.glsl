#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D Texture0;

uniform vec3 colorMult = vec3(1.0);

void main()
{
	vec4 texColor = texture(Texture0,TexCoord);
	FragColor = texColor * vec4(colorMult,1.0);
}