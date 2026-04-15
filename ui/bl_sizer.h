#pragma once

#include <memory>
#include <vector>

namespace bl {

class blWidget;

class blSizer {
 public:
  virtual ~blSizer() = default;

  virtual void Layout(blWidget* parent) = 0;
  virtual void Add(const std::shared_ptr<blWidget>& child, int proportion = 0) = 0;
  virtual std::vector<std::shared_ptr<blWidget>> GetChildren() const = 0;
};

}  // namespace bl
