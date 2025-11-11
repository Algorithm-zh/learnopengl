#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>

#include <iostream>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);

const unsigned int SRC_WIDTH = 1920;
const unsigned int SRC_HEIGHT = 1080;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SRC_WIDTH / 2.0f;
float lastY = SRC_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastTime = 0.0f;

int main (int argc, char *argv[]) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  auto window = glfwCreateWindow(SRC_WIDTH, SRC_HEIGHT, "spot soft light", nullptr, nullptr);
  if(window == nullptr){
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  //设置上下文为当前上下文
  glfwMakeContextCurrent(window);
  //设置帧缓冲回调
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  //设置鼠标捕捉回调
  glfwSetCursorPosCallback(window, mouse_callback);
  //设置鼠标滚轮回调
  glfwSetScrollCallback(window, scroll_callback);

  //告诉glfw去捕捉鼠标
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  //加载所有的opengl函数指针
  if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  //配置深度测试(避免后面的物体错误的覆盖掉前面的物体)
  glEnable(GL_DEPTH_TEST);

  //build and compile 着色器程序
  Shader lightingShader("5.4.light_casters.vs", "5.4.light_casters.fs");
  Shader lightCubeShader("5.4.light_cube.vs", "5.4.light_cube.fs");


  // set up vertex data (and buffer(s)) and configure vertex attributes
  // ------------------------------------------------------------------
  float vertices[] = {
    // positions          // normals           // texture coords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
    0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
    0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
    0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
    0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
    0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
    0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
    0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
  };
  // positions all containers
  glm::vec3 cubePositions[] = {
    glm::vec3( 0.0f,  0.0f,  0.0f),
    glm::vec3( 2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3( 2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3( 1.3f, -2.0f, -2.5f),
    glm::vec3( 1.5f,  2.0f, -2.5f),
    glm::vec3( 1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
  };

  //创建VBO和方块VAO
  unsigned int VBO, cubeVAO;
  glGenVertexArrays(1, &cubeVAO);
  glGenBuffers(1, &VBO);

  //将所有的顶点数据全部copy到VBO里存储
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindVertexArray(cubeVAO);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  //创建灯光的VAO，VBO用之前创建的即可
  unsigned int lightCubeVAO;
  glGenVertexArrays(1, &lightCubeVAO);
  glBindVertexArray(lightCubeVAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  //加载纹理
  unsigned int diffuseMap = loadTexture(FileSystem::getPath("resources/textures/container2.png").c_str());
  unsigned int specularMap = loadTexture(FileSystem::getPath("resources/textures/container2_specular.png").c_str());

  //配置着色器纹理
  lightingShader.use();
  lightingShader.setInt("material.diffuse", 0);
  lightingShader.setInt("material.specular", 1);

  while(!glfwWindowShouldClose(window)){
    
    //计算时间
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastTime;
    lastTime = currentFrame;

    processInput(window);

    //清空屏幕
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    lightingShader.use();
    //配置着色器
    //设置手电筒
    //设置摄像机坐标
    lightingShader.setVec3("light.position", camera.Position);
    //设置摄像机前轴(即视角)
    lightingShader.setVec3("light.direction", camera.Front);
    //设置光切角内角
    lightingShader.setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
    //设置光切角外角
    lightingShader.setFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));
    //设置摄像头位置
    lightingShader.setVec3("viewPos", camera.Position);

    //设置环境光、漫反射和镜面光强度
    lightingShader.setVec3("light.ambient", 0.1f, 0.1f, 0.1f);
    lightingShader.setVec3("light.diffuse", 0.8f, 0.8f, 0.8f);
    lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
    //设置衰减公式的常数项、一次项、二次项的值
    lightingShader.setFloat("light.constant", 1.0f);
    lightingShader.setFloat("light.linear", 0.09f);
    lightingShader.setFloat("light.quadratic", 0.032f);
    //设置材质的反光度
    lightingShader.setFloat("material.shininess", 32.0f);

    //创建透视投影矩阵(视野，宽高比，近远平面)和观察矩阵(摄像机)
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SRC_WIDTH / SRC_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    lightingShader.setMat4("projection", projection);
    lightingShader.setMat4("view", view);
    //创建模型矩阵
    glm::mat4 model = glm::mat4(1.0f);
    lightingShader.setMat4("model", model);

    //绑定漫反射纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);
    //绑定镜面反射纹理
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specularMap);
    //绑定VAO开始绘制
    glBindVertexArray(cubeVAO);
    for(unsigned int i = 0; i < 10; i ++){
      //创建模型矩阵
      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, cubePositions[i]);
      float angle = 20.0f * i;
      model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
      lightingShader.setMat4("model", model);
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glfwSwapBuffers(window);
    glfwPollEvents();
    
  }
  glDeleteVertexArrays(1, &cubeVAO);
  glDeleteVertexArrays(1, &lightCubeVAO);
  glDeleteBuffers(1, &VBO);

  glfwTerminate();

  return 0;
}
 
void framebuffer_size_callback(GLFWwindow* window, int width, int height)  {
  glViewport(0, 0, width, height); 
}
 
//鼠标移动回调
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)  {
  float xpos = static_cast<float>(xposIn); 
  float ypos = static_cast<float>(yposIn); 

  //如果是刚加载的话先设置xy坐标，不然画面会闪
  if(firstMouse){
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }
  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos;//y轴反转(因为y坐标是从底部往顶部依次增大的)

  lastX = xpos;
  lastY = ypos;

  camera.ProcessMouseMovement(xoffset, yoffset);
}
 
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)  {
  camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
 
void processInput(GLFWwindow* window)  {
 
  if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.ProcessKeyboard(FORWARD, deltaTime);
  if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.ProcessKeyboard(BACKWARD, deltaTime);
  if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.ProcessKeyboard(LEFT, deltaTime);
  if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.ProcessKeyboard(RIGHT, deltaTime);
}
 
//加载纹理
unsigned int loadTexture(const char* path)  {

  unsigned int textureID;
  glGenTextures(1, &textureID);
  int width, height, nrComponents;
  auto data = stbi_load(path, &width, &height, &nrComponents, 0);
  if(data){
    GLenum format;
    if(nrComponents == 1){
      format = GL_RED;
    }else if(nrComponents == 3){
      format = GL_RGB;
    }else if(nrComponents == 4){
      format = GL_RGBA;
    }
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  }else{
    std::cout << "Texture failed to load at path:" << path << std::endl;
    stbi_image_free(data);
  }

  return textureID;
}
