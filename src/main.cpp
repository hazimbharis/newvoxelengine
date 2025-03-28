#include <glad/glad.h>

// Maths
#include <glm/common.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Window library
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

// Quick GUI
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

// Loading image formats
#define STB_IMAGE_IMPLEMENTATION //modifies header file so only contains the relevant definition source code
#include <stb_image.h>

// Loading text-based files
#include <iostream>
#include <fstream>
#include <sstream>

void onStart()
{
  
}

int LoadShader(const std::string shaderFilePath, GLenum shaderType)
{
  std::ifstream shaderFile(shaderFilePath);
  if (!shaderFile.is_open()) {
      std::cerr << "Failed to open shader file: " << shaderFilePath << std::endl;
      return -1;
  }

  std::stringstream shaderStream;
  shaderStream << shaderFile.rdbuf(); // Read file contents
  std::string shaderCode = shaderStream.str();
  const char* shaderSource = shaderCode.c_str();

  // Create and compile the shader
  unsigned int shaderID = glCreateShader(shaderType);
  glShaderSource(shaderID, 1, &shaderSource, nullptr);
  glCompileShader(shaderID);

  // Check for compilation errors
  int success;
  char infoLog[512];
  glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
  if (!success) {
      glGetShaderInfoLog(shaderID, 512, nullptr, infoLog);
      std::cerr << "Shader compilation error (" << shaderFilePath << "):\n" << infoLog << std::endl;
      return -1;
  }

  return shaderID;
}

// Debug message function for when something goes wrong
void GLAPIENTRY DebugMessageCallback(
  GLenum source,
  GLenum type,
  GLuint id,
  GLenum severity,
  [[maybe_unused]] GLsizei length,
  const GLchar* message,
  [[maybe_unused]] const void* userParam)
{
  // Ignore certain verbose info messages (particularly ones on Nvidia).
  if (id == 131169 || 
      id == 131185 || // NV: Buffer will use video memory
      id == 131218 || 
      id == 131204 || // Texture cannot be used for texture mapping
      id == 131222 ||
      id == 131154 || // NV: pixel transfer is synchronized with 3D rendering
      id == 0         // gl{Push, Pop}DebugGroup
  )
  return;

  std::stringstream debugMessageStream;
  debugMessageStream << message << '\n';

  switch (source)
  {
      case GL_DEBUG_SOURCE_API: debugMessageStream << "Source: API"; break;
      case GL_DEBUG_SOURCE_WINDOW_SYSTEM: debugMessageStream << "Source: Window Manager"; break;
      case GL_DEBUG_SOURCE_SHADER_COMPILER: debugMessageStream << "Source: Shader Compiler"; break;
      case GL_DEBUG_SOURCE_THIRD_PARTY: debugMessageStream << "Source: Third Party"; break;
      case GL_DEBUG_SOURCE_APPLICATION: debugMessageStream << "Source: Application"; break;
      case GL_DEBUG_SOURCE_OTHER: debugMessageStream << "Source: Other"; break;
  }

  debugMessageStream << '\n';

  switch (type)
  {
      case GL_DEBUG_TYPE_ERROR: debugMessageStream << "Type: Error"; break;
      case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: debugMessageStream << "Type: Deprecated Behaviour"; break;
      case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: debugMessageStream << "Type: Undefined Behaviour"; break;
      case GL_DEBUG_TYPE_PORTABILITY: debugMessageStream << "Type: Portability"; break;
      case GL_DEBUG_TYPE_PERFORMANCE: debugMessageStream << "Type: Performance"; break;
      case GL_DEBUG_TYPE_MARKER: debugMessageStream << "Type: Marker"; break;
      case GL_DEBUG_TYPE_PUSH_GROUP: debugMessageStream << "Type: Push Group"; break;
      case GL_DEBUG_TYPE_POP_GROUP: debugMessageStream << "Type: Pop Group"; break;
      case GL_DEBUG_TYPE_OTHER: debugMessageStream << "Type: Other"; break;
  }

  debugMessageStream << '\n';

  switch (severity)
  {
      case GL_DEBUG_SEVERITY_HIGH: debugMessageStream << "Severity: high"; break;
      case GL_DEBUG_SEVERITY_MEDIUM: debugMessageStream << "Severity: medium"; break;
      case GL_DEBUG_SEVERITY_LOW: debugMessageStream << "Severity: low"; break;
      case GL_DEBUG_SEVERITY_NOTIFICATION: debugMessageStream << "Severity: notification"; break;
  }

    std::cout << "OpenGL Message:" << type << debugMessageStream.str() << std::endl;
}

void renderTriangle(float rot, unsigned int shaderProgram, unsigned int VAO, unsigned int texture)
{
  // 2. use our shader program when we want to render an object
  glUseProgram(shaderProgram);

  glm::mat4 vec = glm::mat4(1.0f);
  //vec = glm::translate(vec, glm::vec3(1.0f, 1.0f, 0.0f));
  //vec = glm::rotate(vec, glm::radians(rot), glm::vec3(0.0f, 0.0f, 1.0f));
  vec = glm::rotate(vec, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
  //vec = glm::scale(vec, glm::vec3(0.5, 0.5, 0.5));  
  unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
  glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(vec));

  // 2.5 bind texture before drawing
  glBindTexture(GL_TEXTURE_2D, texture);
  // 3. bind the VAO we're going to use to switch between different vertex arrays easy
  glBindVertexArray(VAO);
  // 4. now draw the object 
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  // 5. unbind vao
  glBindVertexArray(0);
}

int main()
{
  GLFWwindow* window;

  if (!glfwInit())
  {
    return -1;
  }

  window = glfwCreateWindow(1280, 820, "zim-engine", nullptr, nullptr);

  if (!window)
  {
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD! " << "from zim engine" << std::endl;

  }


  float red = 0;
  float delta = 0.01;


  // setup imgui
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 150");
  ImGui::StyleColorsDark();

  // glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
  // glm::mat4 trans = glm::mat4(1.0f);
  // trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 0.0f));
  // trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
  // trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));  
  // vec = trans * vec;
  // std::cout << vec.x << vec.y << vec.z << std::endl;

  // Initialize all the variables
  float vertices[] = {
    -0.5f, -0.75f, 0.0f, 0.0f, 0.0f,  // lower-left corner  
     0.5f, -0.75f, 0.0f, 1.0f, 0.0f,  // lower-right corner   
     0.5f,  0.75f, 0.0f, 1.0f, 1.0f,  // top-right corner 
     -0.5f,  0.75f, 0.0f, 0.0f, 1.0f  // top-left corner 
  };

  unsigned int indices[] = {  // note that we start from 0!
    0, 1, 3,   // first triangle
    1, 2, 3    // second triangle
  };  

  // Run the initialize buffer functions
  unsigned int VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  unsigned int VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);  
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  unsigned int EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);  
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  // Load shaders
  int vertexShader = LoadShader("C:/Users/azrom/Documents/GitHub/newvoxelengine/src/vertex_shader.glsl", GL_VERTEX_SHADER);
  int fragmentShader = LoadShader("C:/Users/azrom/Documents/GitHub/newvoxelengine/src/fragment_shader1.glsl", GL_FRAGMENT_SHADER);

  // Create shaderProgram
  unsigned int shaderProgram;
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  // Delete shaders
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  // Set format for vertexes
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  // Set format for texture coordinates
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // Set texture parameters
  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // Load texture
  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(true);  
  unsigned char *data = stbi_load("C:\\Users\\azrom\\Documents\\GitHub\\newvoxelengine\\textures\\GfLpMT4asAYLY8J.png", &width, &height, &nrChannels, 0);
  if (data) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
  } else {
      std::cout << "Failed to load texture" << std::endl;
  }
  stbi_image_free(data);


  glDebugMessageCallback(DebugMessageCallback, nullptr);
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  
  float rotation = 0.0f;
  while (!glfwWindowShouldClose(window))
  {
    glClear(GL_COLOR_BUFFER_BIT);


    red = delta;
    if (glm::abs(red) > 0.99)
    {
      delta *= -1;  
    }
      
    glClearColor(red, 0.0, 0.0, 1.0);

    renderTriangle(rotation, shaderProgram, VAO, texture);
    rotation += 0.01f;


    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    {
      static uint32_t counter = 0;
      ImGui::Begin("zim-engine");
        ImGui::Text("frame counter: %d", counter);
      ImGui::End();

      ImGui::ShowDemoWindow();
      counter++;
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  
  glfwTerminate();
}
