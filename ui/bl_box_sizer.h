#pragma once

#include <memory>
#include <vector>

#include "ui/bl_sizer.h"

namespace bl {

class blWidget;

class blBoxSizer : public blSizer {
 public:
  enum class Orientation {
    Horizontal,
    Vertical
  };

  explicit blBoxSizer(Orientation orientation);

  void Layout(blWidget* parent) override;
  void Add(const std::shared_ptr<blWidget>& child, int proportion = 0) override;
  std::vector<std::shared_ptr<blWidget>> GetChildren() const override;

 private:
  struct Item {
    std::shared_ptr<blWidget> child;
    int proportion = 0;
  };

  Orientation orientation_;
  std::vector<Item> items_;
};

}  // namespace bl
