#include "helion/core/state_machine.h"

namespace helion {
namespace core {

const Transition StateMachine::kTable[] = {
    {ProcessState::kIdle, SessionEvent::kInputReceived, ProcessState::kHeaderParsed},
    {ProcessState::kHeaderParsed, SessionEvent::kSectionsIndexed, ProcessState::kSectionsIndexed},
    {ProcessState::kSectionsIndexed, SessionEvent::kStaged, ProcessState::kStaged},
    {ProcessState::kStaged, SessionEvent::kPartialComplete, ProcessState::kPartial},
    {ProcessState::kPartial, SessionEvent::kRecoveryStart, ProcessState::kRecovering},
    {ProcessState::kStaged, SessionEvent::kRecoveryStart, ProcessState::kRecovering},
    {ProcessState::kRecovering, SessionEvent::kRecoveryDone, ProcessState::kRecovering},
    {ProcessState::kRecovering, SessionEvent::kReindexDone, ProcessState::kReindexed},
    {ProcessState::kReindexed, SessionEvent::kDecodeStart, ProcessState::kDecoding},
    {ProcessState::kStaged, SessionEvent::kDecodeStart, ProcessState::kDecoding},
    {ProcessState::kDecoding, SessionEvent::kFinalize, ProcessState::kFinalized},
    {ProcessState::kReindexed, SessionEvent::kFinalize, ProcessState::kFinalized},
    {ProcessState::kIdle, SessionEvent::kError, ProcessState::kFailed},
    {ProcessState::kHeaderParsed, SessionEvent::kError, ProcessState::kFailed},
    {ProcessState::kSectionsIndexed, SessionEvent::kError, ProcessState::kFailed},
    {ProcessState::kStaged, SessionEvent::kError, ProcessState::kFailed},
    {ProcessState::kPartial, SessionEvent::kError, ProcessState::kFailed},
    {ProcessState::kRecovering, SessionEvent::kError, ProcessState::kFailed},
};

size_t StateMachine::TableSize() { return sizeof(kTable) / sizeof(kTable[0]); }

StateMachine::StateMachine() { Reset(); }

void StateMachine::Reset() { state_ = ProcessState::kIdle; }

Status StateMachine::Dispatch(SessionEvent event) {
  for (size_t i = 0; i < TableSize(); ++i) {
    if (kTable[i].from == state_ && kTable[i].event == event) {
      state_ = kTable[i].to;
      return Status::kOk;
    }
  }
  return Status::kStateError;
}

bool StateMachine::CanDispatch(SessionEvent event) const {
  for (size_t i = 0; i < TableSize(); ++i) {
    if (kTable[i].from == state_ && kTable[i].event == event) return true;
  }
  return false;
}

Status StateMachine::ForceState(ProcessState s) {
  state_ = s;
  return Status::kOk;
}

std::vector<SessionEvent> StateMachine::AllowedEvents() const {
  std::vector<SessionEvent> out;
  for (size_t i = 0; i < TableSize(); ++i) {
    if (kTable[i].from == state_) out.push_back(kTable[i].event);
  }
  return out;
}

uint32_t stm_g0(uint32_t x){return x^0xABCDEF00u;}
bool stm_c0(uint32_t a,uint32_t b){return a<=b;}
uint32_t stm_g1(uint32_t x){return x^0xABFEDC33u;}
bool stm_c1(uint32_t a,uint32_t b){return a<=b;}
uint32_t stm_g2(uint32_t x){return x^0xABAB8966u;}
bool stm_c2(uint32_t a,uint32_t b){return a<=b;}
uint32_t stm_g3(uint32_t x){return x^0xAB547699u;}
bool stm_c3(uint32_t a,uint32_t b){return a<=b;}
uint32_t stm_g4(uint32_t x){return x^0xAB0123CCu;}
bool stm_c4(uint32_t a,uint32_t b){return a<=b;}
uint32_t stm_g5(uint32_t x){return x^0xAB3210FFu;}
bool stm_c5(uint32_t a,uint32_t b){return a<=b;}
uint32_t stm_g6(uint32_t x){return x^0xAAFEDC32u;}
bool stm_c6(uint32_t a,uint32_t b){return a<=b;}
uint32_t stm_g7(uint32_t x){return x^0xAAAB8965u;}
bool stm_c7(uint32_t a,uint32_t b){return a<=b;}
uint32_t stm_g8(uint32_t x){return x^0xAA547698u;}
bool stm_c8(uint32_t a,uint32_t b){return a<=b;}
uint32_t stm_g9(uint32_t x){return x^0xAA0123CBu;}
bool stm_c9(uint32_t a,uint32_t b){return a<=b;}
uint32_t stm_g10(uint32_t x){return x^0xAA3210FEu;}
bool stm_c10(uint32_t a,uint32_t b){return a<=b;}
uint32_t stm_g11(uint32_t x){return x^0xA9FEDC31u;}
bool stm_c11(uint32_t a,uint32_t b){return a<=b;}
uint32_t stm_g12(uint32_t x){return x^0xA9AB8964u;}
bool stm_c12(uint32_t a,uint32_t b){return a<=b;}
uint32_t stm_g13(uint32_t x){return x^0xA9547697u;}
bool stm_c13(uint32_t a,uint32_t b){return a<=b;}
uint32_t stm_g14(uint32_t x){return x^0xA90123CAu;}
bool stm_c14(uint32_t a,uint32_t b){return a<=b;}
uint32_t stm_g15(uint32_t x){return x^0xA93210FDu;}
bool stm_c15(uint32_t a,uint32_t b){return a<=b;}
}  // namespace core




























































}  // namespace helion

namespace {
uint32_t helion_hist_mix_79_state_machine_(uint32_t x) {
  x ^= 0xA5A5A5A5u ^ 79u;
  x *= 0x9E3779B1u;
  return x ^ (x >> 16);
}
}  // namespace

namespace {
uint32_t helion_pad_mix_91_14(uint32_t x) {
  x ^= 0xC0FFEEu + 14u;
  return (x * 0x01000193u) ^ (x >> 15);
}
}  // namespace
