#include "rendering/bl_render_context.h"

#include <algorithm>
#include <stdexcept>

#if defined(_WIN32)
#include <windows.h>
#endif
#include <GL/gl.h>
#ifndef GL_BGRA
#define GL_BGRA 0x80E1
#endif

namespace bl {

blRenderContext::~blRenderContext() { Shutdown(); }

bool blRenderContext::Initialize(GLFWwindow* window, int width, int height) {
  if (!window || width <= 0 || height <= 0) {
    return false;
  }
  window_ = window;

  if (!RecreateFramebuffer(width, height)) {
    return false;
  }
  if (!EnsureTexture()) {
    return false;
  }
  return true;
}

void blRenderContext::Shutdown() {
  ctx_.reset();
  framebuffer_.reset();
  if (texture_id_ != 0U) {
    glDeleteTextures(1, &texture_id_);
    texture_id_ = 0;
  }
  width_ = 0;
  height_ = 0;
  window_ = nullptr;
}

bool blRenderContext::Resize(int width, int height) {
  if (width <= 0 || height <= 0) {
    return false;
  }
  if (width == width_ && height == height_) {
    return true;
  }
  if (!RecreateFramebuffer(width, height)) {
    return false;
  }
  return EnsureTexture();
}

bool blRenderContext::BeginFrame(BLRgba32 clear_color) {
  return BeginFrame(clear_color, std::vector<BLRect>{});
}

bool blRenderContext::BeginFrame(BLRgba32 clear_color, const std::vector<BLRect>& dirty_regions) {
  if (width_ <= 0 || height_ <= 0) {
    return false;
  }
  ctx_ = std::make_unique<BLContext>(framebuffer_);
  if (!ctx_ || !ctx_->is_valid()) {
    return false;
  }

  ctx_->set_comp_op(BL_COMP_OP_SRC_COPY);
  if (dirty_regions.empty()) {
    ctx_->fill_all(clear_color);
    return true;
  }

  for (const BLRect& region : dirty_regions) {
    const double x = (std::max)(0.0, region.x);
    const double y = (std::max)(0.0, region.y);
    const double right = (std::min)(static_cast<double>(width_), region.x + region.w);
    const double bottom = (std::min)(static_cast<double>(height_), region.y + region.h);
    if (right <= x || bottom <= y) {
      continue;
    }
    ctx_->fill_rect(BLRect(x, y, right - x, bottom - y), clear_color);
  }
  return true;
}

BLContext& blRenderContext::GetContext() {
  if (!ctx_) {
    throw std::runtime_error("BLContext requested outside BeginFrame/EndFrame scope.");
  }
  return *ctx_;
}

void blRenderContext::EndFrame() {
  if (ctx_) {
    ctx_->end();
    ctx_.reset();
  }
}

void blRenderContext::Present() {
  if (texture_id_ == 0U || width_ <= 0 || height_ <= 0) {
    return;
  }

  BLImageData image_data{};
  if (framebuffer_.get_data(&image_data) != BL_SUCCESS) {
    return;
  }

  glViewport(0, 0, width_, height_);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  glBindTexture(GL_TEXTURE_2D, texture_id_);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
  glTexSubImage2D(
      GL_TEXTURE_2D,
      0,
      0,
      0,
      width_,
      height_,
      GL_BGRA,
      GL_UNSIGNED_BYTE,
      image_data.pixel_data);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glEnable(GL_TEXTURE_2D);
  glBegin(GL_QUADS);
  glTexCoord2f(0.0F, 1.0F);
  glVertex2f(-1.0F, -1.0F);
  glTexCoord2f(1.0F, 1.0F);
  glVertex2f(1.0F, -1.0F);
  glTexCoord2f(1.0F, 0.0F);
  glVertex2f(1.0F, 1.0F);
  glTexCoord2f(0.0F, 0.0F);
  glVertex2f(-1.0F, 1.0F);
  glEnd();
  glDisable(GL_TEXTURE_2D);
}

bool blRenderContext::EnsureTexture() {
  if (texture_id_ == 0U) {
    glGenTextures(1, &texture_id_);
  }
  if (texture_id_ == 0U) {
    return false;
  }

  glBindTexture(GL_TEXTURE_2D, texture_id_);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_RGBA8,
      width_,
      height_,
      0,
      GL_BGRA,
      GL_UNSIGNED_BYTE,
      nullptr);
  return true;
}

bool blRenderContext::RecreateFramebuffer(int width, int height) {
  width_ = width;
  height_ = height;
  framebuffer_.reset();
  const BLResult result = framebuffer_.create(width_, height_, BL_FORMAT_PRGB32);
  return result == BL_SUCCESS;
}

}  // namespace bl
