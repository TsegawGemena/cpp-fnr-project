#pragma once
#include "helion/status.h"
#include <functional>
#include <queue>
#include <vector>

namespace helion {
namespace runtime {

using TaskFn = std::function<Status()>;

struct Task {
  uint32_t id = 0;
  TaskFn fn;
  int priority = 0;
};

class Executor {
 public:
  void Enqueue(Task task);
  Status RunAll();
  Status RunNext();
  void Clear();
  size_t Pending() const { return queue_.size(); }
  size_t Completed() const { return completed_; }

 private:
  struct Compare {
    bool operator()(const Task& a, const Task& b) const { return a.priority < b.priority; }
  };
  std::priority_queue<Task, std::vector<Task>, Compare> queue_;
  size_t completed_ = 0;
  uint32_t next_id_ = 1;
};

}  // namespace runtime
}  // namespace helion

