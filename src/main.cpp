#include <glad/glad.h>

#include <glm/common.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <iostream>


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

  
  while (!glfwWindowShouldClose(window))
  {
    glClear(GL_COLOR_BUFFER_BIT);


    red = delta;
    if (glm::abs(red) > 0.99)
    {
      delta *= -1;  
    }
      
    glClearColor(red, 0.0, 0.0, 1.0);


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
