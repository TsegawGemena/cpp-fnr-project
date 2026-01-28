#pragma once
#include "helion/core/state_machine.h"
#include <functional>
#include <vector>

namespace helion {
namespace runtime {

using EventHandler = std::function<void(core::SessionEvent)>;

class EventBus {
 public:
  void Subscribe(EventHandler handler);
  void Publish(core::SessionEvent event);
  void Clear();
  size_t SubscriberCount() const { return handlers_.size(); }
  const std::vector<core::SessionEvent>& History() const { return history_; }

 private:
  std::vector<EventHandler> handlers_;
  std::vector<core::SessionEvent> history_;
};

}  // namespace runtime
}  // namespace helion
