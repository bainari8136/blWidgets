#include "platform/bl_window.h"

#include <utility>

namespace bl {

blWindow::~blWindow() { Destroy(); }

bool blWindow::Create(int width, int height, const char* title) {
  if (!glfwInit()) {
    return false;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

  window_ = glfwCreateWindow(width, height, title, nullptr, nullptr);
  if (!window_) {
    glfwTerminate();
    return false;
  }

  glfwMakeContextCurrent(window_);
  glfwSwapInterval(1);

  glfwSetWindowUserPointer(window_, this);
  glfwSetCursorPosCallback(window_, CursorPosCallback);
  glfwSetMouseButtonCallback(window_, MouseButtonCallback);
  glfwSetFramebufferSizeCallback(window_, FramebufferSizeCallback);
  glfwSetWindowCloseCallback(window_, WindowCloseCallback);
  glfwSetKeyCallback(window_, KeyCallback);
  glfwSetCharCallback(window_, CharCallback);

  glfwGetFramebufferSize(window_, &framebuffer_width_, &framebuffer_height_);
  PushResizeEvent(framebuffer_width_, framebuffer_height_);
  running_ = true;
  return true;
}

void blWindow::Destroy() {
  if (window_) {
    glfwDestroyWindow(window_);
    window_ = nullptr;
  }
  glfwTerminate();
  running_ = false;
}

void blWindow::PollEvents() {
  if (!window_) {
    running_ = false;
    return;
  }
  glfwPollEvents();
  if (glfwWindowShouldClose(window_)) {
    running_ = false;
  }
}

void blWindow::SwapBuffers() {
  if (window_) {
    glfwSwapBuffers(window_);
  }
}

std::vector<Event> blWindow::ConsumeEvents() {
  std::vector<Event> out = std::move(events_);
  events_.clear();
  return out;
}

bool blWindow::IsRunning() const { return running_; }

void blWindow::RequestClose() {
  running_ = false;
  if (window_) {
    glfwSetWindowShouldClose(window_, GLFW_TRUE);
  }
}

GLFWwindow* blWindow::NativeHandle() const { return window_; }

int blWindow::FramebufferWidth() const { return framebuffer_width_; }

int blWindow::FramebufferHeight() const { return framebuffer_height_; }

void blWindow::CursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
  auto* self = static_cast<blWindow*>(glfwGetWindowUserPointer(window));
  if (!self) {
    return;
  }
  self->PushMouseEvent(EventType::MouseMove, xpos, ypos, 0);
}

void blWindow::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
  (void)mods;
  auto* self = static_cast<blWindow*>(glfwGetWindowUserPointer(window));
  if (!self) {
    return;
  }

  double x = 0.0;
  double y = 0.0;
  glfwGetCursorPos(window, &x, &y);

  if (action == GLFW_PRESS) {
    self->PushMouseEvent(EventType::MouseDown, x, y, button);
  } else if (action == GLFW_RELEASE) {
    self->PushMouseEvent(EventType::MouseUp, x, y, button);
  }
}

void blWindow::FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
  auto* self = static_cast<blWindow*>(glfwGetWindowUserPointer(window));
  if (!self) {
    return;
  }
  self->framebuffer_width_ = width;
  self->framebuffer_height_ = height;
  self->PushResizeEvent(width, height);
}

void blWindow::WindowCloseCallback(GLFWwindow* window) {
  auto* self = static_cast<blWindow*>(glfwGetWindowUserPointer(window));
  if (!self) {
    return;
  }
  self->running_ = false;
  self->PushCloseEvent();
}

void blWindow::PushMouseEvent(EventType type, double x, double y, int button) {
  if (!window_) {
    return;
  }

  int window_width = 0;
  int window_height = 0;
  glfwGetWindowSize(window_, &window_width, &window_height);

  float scale_x = 1.0F;
  float scale_y = 1.0F;
  if (window_width > 0 && window_height > 0) {
    scale_x = static_cast<float>(framebuffer_width_) / static_cast<float>(window_width);
    scale_y = static_cast<float>(framebuffer_height_) / static_cast<float>(window_height);
  }

  Event evt{};
  evt.type = type;
  evt.x = static_cast<float>(x) * scale_x;
  evt.y = static_cast<float>(y) * scale_y;
  evt.button = button;
  events_.push_back(evt);
}

void blWindow::PushResizeEvent(int width, int height) {
  Event evt{};
  evt.type = EventType::WindowResized;
  evt.width = width;
  evt.height = height;
  events_.push_back(evt);
}

void blWindow::PushCloseEvent() {
  Event evt{};
  evt.type = EventType::WindowClose;
  events_.push_back(evt);
}

void blWindow::PushKeyEvent(EventType type, const std::string& key) {
  Event evt{};
  evt.type = type;
  evt.key = key;
  events_.push_back(evt);
}

void blWindow::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  (void)scancode;
  (void)mods;
  auto* self = static_cast<blWindow*>(glfwGetWindowUserPointer(window));
  if (!self) {
    return;
  }

  if (action == GLFW_PRESS || action == GLFW_REPEAT) {
    std::string key_str;
    switch (key) {
      case GLFW_KEY_BACKSPACE: key_str = "Backspace"; break;
      case GLFW_KEY_DELETE: key_str = "Delete"; break;
      case GLFW_KEY_LEFT: key_str = "Left"; break;
      case GLFW_KEY_RIGHT: key_str = "Right"; break;
      case GLFW_KEY_HOME: key_str = "Home"; break;
      case GLFW_KEY_END: key_str = "End"; break;
      default: return; // Handle actual characters via CharCallback
    }
    self->PushKeyEvent(EventType::KeyDown, key_str);
  }
}

void blWindow::CharCallback(GLFWwindow* window, unsigned int codepoint) {
  auto* self = static_cast<blWindow*>(glfwGetWindowUserPointer(window));
  if (!self) {
    return;
  }

  std::string utf8_char;
  if (codepoint <= 0x7F) {
      utf8_char.push_back(static_cast<char>(codepoint));
  } else if (codepoint <= 0x7FF) {
      utf8_char.push_back(static_cast<char>(0xC0 | ((codepoint >> 6) & 0x1F)));
      utf8_char.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
  } else if (codepoint <= 0xFFFF) {
      utf8_char.push_back(static_cast<char>(0xE0 | ((codepoint >> 12) & 0x0F)));
      utf8_char.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
      utf8_char.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
  } else if (codepoint <= 0x10FFFF) {
      utf8_char.push_back(static_cast<char>(0xF0 | ((codepoint >> 18) & 0x07)));
      utf8_char.push_back(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F)));
      utf8_char.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
      utf8_char.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
  }

  if (!utf8_char.empty()) {
      self->PushKeyEvent(EventType::KeyDown, utf8_char);
  }
}

}  // namespace bl
