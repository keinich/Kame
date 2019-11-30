#include "kmpch.h"
#include "VulkanTest.h"

#include "GLFW/glfw3.h"
#include <cassert>


#include <vulkan/vulkan.h>

void VulkanTest::Run() {

  int rc = glfwInit();
  assert(rc);

  GLFWwindow* window = glfwCreateWindow(1024, 768, "niagara", 0, 0);
  assert(window);

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
  }

  glfwDestroyWindow(window);

}
