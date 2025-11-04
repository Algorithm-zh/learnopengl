#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform float mixValue;

// texture samplers
uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
	// linearly interpolate between both textures 最后一个参数是插值因子，可以调整两个纹理的可见比例
	FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), mixValue);
}
