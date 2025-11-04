#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
  
uniform vec3 lightPos; 
//观察者坐标
uniform vec3 viewPos; 
uniform vec3 lightColor;
uniform vec3 objectColor;

void main()
{
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // specular
    float specularStrength = 0.8;//镜面强度
    vec3 viewDir = normalize(viewPos - FragPos);//观察者视角向量
    //reflect函数要求第一个向量是从光源指向片段位置的向量，第二个参数为法向量
    vec3 reflectDir = reflect(-lightDir, norm);//反射向量
    //计算镜面分量，计算视线方向和反射方向的点乘的32次幂，32是反光度，越高反射能力越强
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  
        
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
} 



