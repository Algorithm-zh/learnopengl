#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;//世界坐标, 即每个片段的世界位置  
  
//光源的位置向量
uniform vec3 lightPos; 
uniform vec3 lightColor;
uniform vec3 objectColor;

void main()
{
    // ambient  环境光照，只需要将光的颜色乘以一个很小的常理环境因子即可
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 计算漫反射需要法向量和定向的光线(光的位置向量和片段的位置向量)
    vec3 norm = normalize(Normal);//法线向量
    vec3 lightDir = normalize(lightPos - FragPos);//光源位置向量-片段位置向量=光的方向向量
    float diff = max(dot(norm, lightDir), 0.0);//点乘得到角度（大于90度会变成负数）
    vec3 diffuse = diff * lightColor;
            
    vec3 result = (ambient + diffuse) * objectColor;
    FragColor = vec4(result, 1.0);
} 
