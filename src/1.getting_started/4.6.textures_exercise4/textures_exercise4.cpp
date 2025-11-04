#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_s.h>


const unsigned int SRC_WIDTH = 800;
const unsigned int SRC_HEIGHT = 600;

//控制两个纹理的插值比例
float mixValue = 0.2f;

void frameBuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow*);

int main (int argc, char *argv[]) {
  
  glfwInit();
  //设置gl和glsl版本
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  //设置core模式
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  //创建glfw窗口
  auto window =glfwCreateWindow(SRC_WIDTH, SRC_HEIGHT, "test", nullptr, nullptr);
  if(!window){
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  //将window对应的上下文绑定到当前线程的当前上下文
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, &frameBuffer_size_callback);

  //调用glad加载器，这之后才能调用gl函数
  if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  //构建并编译着色器程序
  Shader ourShader("4.5.texture.vs", "4.5.texture.fs");

  //创建并配置顶点属性
  float vertices[] = {
    //顶点位置属性                         颜色属性                      纹理属性
    0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
    0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
  };
  //顶点绘制顺序
  unsigned int indices[] = {
    0,1,3, //第一个三角形
    1,2,3  //第二个三角形
  };
  //创建vao vbo ebo
  unsigned int VAO, VBO, EBO;//这是id,独一无二
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  //VAO的作用是存储所有的顶点属性，当你每次绘制物体的时候你只要绑定相应的VAO就可以
  //这样你就只用配置一次顶点属性，不然每次绘制一个物体就需要配置一次顶点属性，非常麻烦
  //调用该方法后，随后的顶点属性调用都会被存储在这个VAO里
  glBindVertexArray(VAO);

  //VBO的作用是管理显存，他会在显存里存储大量顶点
  //这样可以把大批的顶点一次发送给gpu而不是一个顶点一个顶点的发送
  //调用该方法后，使用的任何在GL_ARRAY_BUFFER目标上的缓冲调用都会用来配置VBO
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  //把之前定义的顶点数据复制到缓冲的内存里,最后一个参数是表示数据是否会经常改变
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  
  //EBO的作用是存储opengl要绘制哪些顶点的索引，也就是索引绘制,说白了就是绘制顺序
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


  //接下来是配置顶点属性, 告诉opengl该如何解析而顶点数据（应用到逐个顶点属性上）
  //位置属性
  //位置属性的位置值 属性大小(vec3)  类型  是否标准化  步长  偏移量 
  glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, 8 * sizeof(float), (void*)0);
  glad_glEnableVertexAttribArray(0);//启用顶点属性
  //颜色属性
  glVertexAttribPointer(1, 3, GL_FLOAT,GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  glad_glEnableVertexAttribArray(1);//启用顶点属性
  //纹理属性
  glVertexAttribPointer(2, 2, GL_FLOAT,GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  glad_glEnableVertexAttribArray(2);//启用顶点属性
  //上面配置的这些顶点属性将会被加到VAO里，以后就可以直接绑定VAO直接绑定这些配置好的顶点属性

  
  //加载纹理
  unsigned int texture1, texture2;
  //texture1 
  //生成纹理的数量  然后将其存储在第二个参数的数组里
  glGenTextures(1, &texture1);
  //绑定之后任何的纹理指令都可以配置当前绑定的纹理
  glBindTexture(GL_TEXTURE_2D, texture1);
  //设置纹理的环绕方式
  //重复环绕
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  //设置纹理的过滤方式(就是放大缩小的时候如何取值)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  //加载纹理图片
  int width, height, nrChannels;
  //沿y轴颠倒坐标
  stbi_set_flip_vertically_on_load(true);
  auto data = stbi_load(FileSystem::getPath("resources/textures/container.jpg").c_str(), &width, &height, &nrChannels, 0);
  if(data){
    //生成纹理，也就是当前纹理对象会被附加上纹理图像
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    //自动生成多级渐远纹理
    glGenerateMipmap(GL_TEXTURE_2D);
  }else{
    std::cout << "Failed to load texture" << std::endl;
  }
  stbi_image_free(data);

  //texture12
  //生成纹理的数量  然后将其存储在第二个参数的数组里
  glGenTextures(1, &texture2);
  //绑定之后任何的纹理指令都可以配置当前绑定的纹理
  glBindTexture(GL_TEXTURE_2D, texture2);
  //设置纹理的环绕方式
  //重复环绕
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  //设置纹理的过滤方式(就是放大缩小的时候如何取值)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  //沿y轴颠倒坐标
  stbi_set_flip_vertically_on_load(true);
  data = stbi_load(FileSystem::getPath("resources/textures/awesomeface.png").c_str(), &width, &height, &nrChannels, 0);
  if(data){
    //生成纹理，也就是当前纹理对象会被附加上纹理图像
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    //自动生成多级渐远纹理
    glGenerateMipmap(GL_TEXTURE_2D);
  }else{
    std::cout << "Failed to load texture" << std::endl;
  }
  stbi_image_free(data);

  //使用着色器程序,更寻uniform之前必须先使用程序，因为它是在当前激活的着色器程序里设置uniform的
  ourShader.use();
  //设置着色器程序里的uniform的值
  glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0);
  //设置方式二 
  ourShader.setInt("texture2", 1);


  //render loop  循环检测窗口有没有关闭
  while(!glfwWindowShouldClose(window)){
    //input
    processInput(window);

    glClearColor(0.2f, 0.3f ,0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    //绑定纹理之前先激活纹理单元
    glActiveTexture(GL_TEXTURE0);
    //glBindTexture会绑定到这个纹理到当前激活的纹理单元
    //纹理单元GL_TEXTURE0默认总是被激活
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    //给着色器设置纹理混合值
    ourShader.setFloat("mixValue", mixValue);
    //当要渲染一个物体的时候要使用着色器程序 
    ourShader.use();
    glBindVertexArray(VAO);
    //使用当前绑定的索引缓冲对象中的索引进行绘制
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);

  glfwTerminate();
  return 0;
}


 
void frameBuffer_size_callback(GLFWwindow* window, int width, int height)  {
  //根据窗口大小调整视口大小
  //视口就是opengl要渲染的区域
  glViewport(0, 0, width, height); 
}
 
void processInput(GLFWwindow* window)  {
 
  if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
    glfwSetWindowShouldClose(window, true);
  }

  if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
    mixValue += 0.01f; 
    if(mixValue >= 1.0f){
      mixValue = 1.0f;
    }
  }
  if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
    mixValue -= 0.01f; 
    if(mixValue <= 0.0f){
      mixValue = 0.0f;
    }
  }
}
