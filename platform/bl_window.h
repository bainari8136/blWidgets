#pragma once

#include <vector>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "ui/bl_event.h"

namespace bl {

class blWindow {
 public:
  blWindow() = default;
  ~blWindow();

  blWindow(const blWindow&) = delete;
  blWindow& operator=(const blWindow&) = delete;

  bool Create(int width, int height, const char* title);
  void Destroy();

  void PollEvents();
  void SwapBuffers();
  std::vector<Event> ConsumeEvents();

  bool IsRunning() const;
  void RequestClose();

  GLFWwindow* NativeHandle() const;
  int FramebufferWidth() const;
  int FramebufferHeight() const;

 private:
  static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
  static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
  static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
  static void WindowCloseCallback(GLFWwindow* window);
  static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
  static void CharCallback(GLFWwindow* window, unsigned int codepoint);

  void PushMouseEvent(EventType type, double x, double y, int button);
  void PushResizeEvent(int width, int height);
  void PushCloseEvent();
  void PushKeyEvent(EventType type, const std::string& key);

  GLFWwindow* window_ = nullptr;
  bool running_ = false;
  int framebuffer_width_ = 0;
  int framebuffer_height_ = 0;
  std::vector<Event> events_;
};

}  // namespace bl
