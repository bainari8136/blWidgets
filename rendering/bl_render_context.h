#pragma once

#include <blend2d/blend2d.h>

#include <memory>
#include <vector>
struct GLFWwindow;

namespace bl {

class blRenderContext {
 public:
  blRenderContext() = default;
  ~blRenderContext();

  blRenderContext(const blRenderContext&) = delete;
  blRenderContext& operator=(const blRenderContext&) = delete;

  bool Initialize(GLFWwindow* window, int width, int height);
  void Shutdown();

  bool Resize(int width, int height);
  bool BeginFrame(BLRgba32 clear_color);
  bool BeginFrame(BLRgba32 clear_color, const std::vector<BLRect>& dirty_regions);
  BLContext& GetContext();
  void EndFrame();
  void Present();

 private:
  bool EnsureTexture();
  bool RecreateFramebuffer(int width, int height);

  GLFWwindow* window_ = nullptr;
  int width_ = 0;
  int height_ = 0;
  BLImage framebuffer_;
  std::unique_ptr<BLContext> ctx_;
  unsigned int texture_id_ = 0;
};

}  // namespace bl
