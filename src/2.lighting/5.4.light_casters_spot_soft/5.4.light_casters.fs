#version 330 core
out vec4 FragColor;

struct Material{
  sampler2D diffuse;
  sampler2D specular;
  float shininess;
};

struct Light{
  vec3 position;
  vec3 direction;
  //切光角
  float cutOff;
  //切光角外边缘
  float outerCutOff;

  //三种光强度
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  //衰减光三项
  float constant;
  float linear;
  float quadratic;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

//摄像头位置
uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
  vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

  //diffuse
  vec3 norm = normalize(Normal);
  //光的方向向量
  vec3 lightDir = normalize(light.position - FragPos);
  //漫反射分量，两个向量之间的角度越大，漫反射分量就越小
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;


  //specular
  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);
  //计算视线方向与反射方向的点乘，取它的 shininess 次幂
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;

  //spotlight
  float theta = dot(lightDir, normalize(-light.direction));
  float epsilon = (light.cutOff - light.outerCutOff);
  //通过公式计算，(光照强度)I=(外面那个角度-外切光角)/内外圆锥之间的余弦差值,通过这个公式
  //得到的结果就是，内圆锥之内的强度都是最高的，内到外逐渐递减，外圆锥外面的都是0
  float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
  //不对环境光处理。不然圆锥外面一点光都没有了
  diffuse *= intensity;
  specular *= intensity;

  //attenuation
  float distance = length(light.position - FragPos);
  float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
  ambient *= attenuation;
  diffuse *= attenuation;
  specular *= attenuation;
  
  vec3 result = ambient + diffuse + specular;
  FragColor = vec4(result, 1.0);
}
