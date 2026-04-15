#pragma once

#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#include <blend2d/blend2d.h>

#include "ui/bl_event.h"
#include "ui/bl_style.h"

namespace bl {

class blWidget {
 public:
  virtual ~blWidget() = default;

  void AddChild(const std::shared_ptr<blWidget>& child) {
    if (!child) {
      return;
    }
    child->parent_ = this;
    children_.push_back(child);
    MarkDirty();
    MarkLayoutConstraintChanged();
  }

  void SetChildren(const std::vector<std::shared_ptr<blWidget>>& children) {
    children_.clear();
    for (const auto& child : children) {
      AddChild(child);
    }
    MarkDirty();
  }

  virtual void Paint(BLContext& ctx) {
    for (const auto& child : children_) {
      if (child) {
        child->Paint(ctx);
      }
    }
    dirty_self_ = false;
    dirty_children_ = false;
  }

  virtual bool OnEvent(const Event& event) {
    bool consumed = false;
    for (auto it = children_.rbegin(); it != children_.rend(); ++it) {
      if (*it) {
        if ((*it)->OnEvent(event)) {
          consumed = true;
          // Stop propagation for click events to prevent clicking through overlapping widgets
          if (event.type == EventType::MouseDown || event.type == EventType::MouseUp) {
            return true;
          }
        }
      }
    }
    return consumed;
  }

  virtual void Update(float deltaTime) {
    for (const auto& child : children_) {
      if (child) {
        child->Update(deltaTime);
      }
    }
  }

  virtual void SetStyle(const blStyle& style) {
    style_ = style;
    MarkDirty();
  }

  const blStyle& GetStyle() const { return style_; }
  virtual const char* TypeName() const { return "widget"; }

  void SetId(const std::string& id) { id_ = id; }
  const std::string& Id() const { return id_; }

  void AddClass(const std::string& class_name) { classes_.insert(class_name); }
  void RemoveClass(const std::string& class_name) { classes_.erase(class_name); }
  bool HasClass(const std::string& class_name) const { return classes_.find(class_name) != classes_.end(); }
  const std::unordered_set<std::string>& Classes() const { return classes_; }

  void SetPosition(float x, float y) {
    if (x_ == x && y_ == y) {
      return;
    }
    x_ = x;
    y_ = y;
    MarkDirty();
    MarkParentDirty();
    MarkLayoutConstraintChanged();
  }

  void SetSize(float width, float height) {
    if (width_ == width && height_ == height) {
      return;
    }
    width_ = width;
    height_ = height;
    MarkDirty();
    MarkParentDirty();
    MarkLayoutConstraintChanged();
  }

  void SetBounds(float x, float y, float width, float height) {
    SetPosition(x, y);
    SetSize(width, height);
  }

  bool HitTest(float x, float y) const {
    return x >= x_ && y >= y_ && x <= x_ + width_ && y <= y_ + height_;
  }

  float X() const { return x_; }
  float Y() const { return y_; }
  float Width() const { return width_; }
  float Height() const { return height_; }
  virtual float PreferredWidth() const { return width_; }
  virtual float PreferredHeight() const { return height_; }
  bool IsDirty() const {
    return dirty_self_ || dirty_children_;
  }
  bool IsSelfDirty() const { return dirty_self_; }
  bool HasLayoutConstraintChanged() const { return layout_constraint_changed_; }
  void ClearLayoutConstraintChanged() { layout_constraint_changed_ = false; }
  void MarkDirty() {
    dirty_self_ = true;
    MarkParentChildDirty();
  }
  void MarkParentDirty() {
    if (parent_) {
      parent_->MarkDirty();
    }
  }
  void ClearDirtyRecursive() {
    dirty_self_ = false;
    dirty_children_ = false;
    for (const auto& child : children_) {
      if (child) {
        child->ClearDirtyRecursive();
      }
    }
  }
  const std::vector<std::shared_ptr<blWidget>>& Children() const { return children_; }

 protected:
  blStyle style_;

 private:
  void MarkParentChildDirty() {
    if (parent_) {
      parent_->MarkChildDirty();
    }
  }

  void MarkChildDirty() {
    dirty_children_ = true;
    MarkParentChildDirty();
  }

  void MarkLayoutConstraintChanged() {
    layout_constraint_changed_ = true;
    if (parent_) {
      parent_->MarkLayoutConstraintChanged();
    }
  }

  blWidget* parent_ = nullptr;
  std::vector<std::shared_ptr<blWidget>> children_;
  bool dirty_self_ = true;
  bool dirty_children_ = false;
  bool layout_constraint_changed_ = false;
  float x_ = 0.0F;
  float y_ = 0.0F;
  float width_ = 0.0F;
  float height_ = 0.0F;
  std::string id_;
  std::unordered_set<std::string> classes_;
};

}  // namespace bl
