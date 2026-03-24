#pragma once
#include "helion/types.h"
#include <vector>

namespace helion {
namespace core {

enum class SessionEvent : uint8_t {
  kInputReceived,
  kHeaderDone,
  kSectionsIndexed,
  kStaged,
  kPartialComplete,
  kRecoveryStart,
  kRecoveryDone,
  kReindexDone,
  kDecodeStart,
  kFinalize,
  kError,
};

struct Transition {
  ProcessState from;
  SessionEvent event;
  ProcessState to;
};

class StateMachine {
 public:
  StateMachine();
  ProcessState State() const { return state_; }
  void Reset();
  Status Dispatch(SessionEvent event);
  bool CanDispatch(SessionEvent event) const;
  Status ForceState(ProcessState s);
  std::vector<SessionEvent> AllowedEvents() const;

 private:
  ProcessState state_ = ProcessState::kIdle;
  static const Transition kTable[];
  static size_t TableSize();
};

}  // namespace core
}  // namespace helion

