#pragma once

#include <memory>
#include <vector>

namespace bl {

class blAnimation {
 public:
  virtual ~blAnimation() = default;

  virtual void Update(float deltaTime) = 0;
  virtual bool IsFinished() const = 0;

  void SetTargetWidget(std::shared_ptr<class blWidget> widget) {
    target_widget_ = widget;
  }

 protected:
  std::weak_ptr<class blWidget> target_widget_;
};

}  // namespace bl