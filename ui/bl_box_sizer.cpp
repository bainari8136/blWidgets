#include "ui/bl_box_sizer.h"

#include <algorithm>

#include "ui/bl_widget.h"

namespace bl {

blBoxSizer::blBoxSizer(Orientation orientation) : orientation_(orientation) {}

void blBoxSizer::Add(const std::shared_ptr<blWidget>& child, int proportion) {
  if (!child) {
    return;
  }
  items_.push_back({child, proportion});
}

std::vector<std::shared_ptr<blWidget>> blBoxSizer::GetChildren() const {
  std::vector<std::shared_ptr<blWidget>> children;
  children.reserve(items_.size());
  for (const Item& item : items_) {
    if (item.child) {
      children.push_back(item.child);
    }
  }
  return children;
}

void blBoxSizer::Layout(blWidget* parent) {
  if (!parent || items_.empty()) {
    return;
  }

  if (orientation_ == Orientation::Vertical) {
    float total_preferred = 0.0F;
    int total_proportion = 0;
    for (const Item& item : items_) {
      if (!item.child) {
        continue;
      }
      total_preferred += std::max(1.0F, item.child->PreferredHeight());
      total_proportion += std::max(0, item.proportion);
    }

    const float extra_space = std::max(0.0F, parent->Height() - total_preferred);
    float y = parent->Y();
    for (const Item& item : items_) {
      if (!item.child) {
        continue;
      }
      float child_height = std::max(1.0F, item.child->PreferredHeight());
      if (item.proportion > 0 && total_proportion > 0) {
        const float share =
            extra_space * (static_cast<float>(item.proportion) / static_cast<float>(total_proportion));
        child_height += share;
      }
      item.child->SetPosition(parent->X(), y);
      item.child->SetSize(parent->Width(), child_height);
      y += child_height;
    }
    return;
  }

  float total_preferred = 0.0F;
  int total_proportion = 0;
  for (const Item& item : items_) {
    if (!item.child) {
      continue;
    }
    total_preferred += std::max(1.0F, item.child->PreferredWidth());
    total_proportion += std::max(0, item.proportion);
  }

  const float extra_space = std::max(0.0F, parent->Width() - total_preferred);
  float x = parent->X();
  for (const Item& item : items_) {
    if (!item.child) {
      continue;
    }
    float child_width = std::max(1.0F, item.child->PreferredWidth());
    if (item.proportion > 0 && total_proportion > 0) {
      const float share = extra_space * (static_cast<float>(item.proportion) / static_cast<float>(total_proportion));
      child_width += share;
    }
    item.child->SetPosition(x, parent->Y());
    item.child->SetSize(child_width, parent->Height());
    x += child_width;
  }
}

}  // namespace bl
