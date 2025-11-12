#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{             
    //加载纹理的全部4个颜色分量
    vec4 texColor = texture(texture1, TexCoords);
    //如果片段的alpha值低于0.1就丢弃它
    if(texColor.a < 0.1)
        discard;
    FragColor = texColor;
}
