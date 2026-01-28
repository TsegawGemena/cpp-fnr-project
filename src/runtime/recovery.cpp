#include "helion/runtime/recovery.h"

namespace helion {
namespace runtime {

RecoveryManager::RecoveryManager(RecoveryContext ctx) : ctx_(ctx) {}

Status RecoveryManager::ReleaseStagingOnly() {
  if (!ctx_.pool) return Status::kStateError;
  ctx_.pool->ReleaseStagingOnly();
  if (ctx_.arena) {
    recovery_gen_ = static_cast<uint32_t>(ctx_.arena->Generation());
  }
  return Status::kOk;
}

Status RecoveryManager::ReleaseStaging() {
  Status st = ReleaseStagingOnly();
  if (Failed(st)) return st;
  return Status::kOk;
}

Status RecoveryManager::ReleasePools() {
  if (!ctx_.pool) return Status::kStateError;
  ctx_.pool->ReleaseAll();
  return Status::kOk;
}

Status RecoveryManager::RewindArena() {
  if (!ctx_.arena) return Status::kStateError;
  ctx_.arena->MarkCheckpoint();
  ctx_.arena->RewindToCheckpoint();
  recovery_gen_ = static_cast<uint32_t>(ctx_.arena->Generation());
  return Status::kOk;
}

Status RecoveryManager::RunRecovery() {
  Status st = ReleaseStagingOnly();
  if (Failed(st)) return st;
  if (ctx_.arena) {
    ctx_.arena->RewindToCheckpoint();
    recovery_gen_ = static_cast<uint32_t>(ctx_.arena->Generation());
  }
  recovered_ = true;
  return Status::kOk;
}

Status RecoveryManager::ResumeAfterRecovery() {
  if (!recovered_) return Status::kRecoveryRequired;
  return Status::kOk;
}

uint32_t recv_r0(uint32_t x){return (x<<3)^0xFEEDFACEu;}
bool recv_s0(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t recv_r1(uint32_t x){return (x<<3)^0xFE9A8DB9u;}
bool recv_s1(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t recv_r2(uint32_t x){return (x<<3)^0xFE031420u;}
bool recv_s2(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t recv_r3(uint32_t x){return (x<<3)^0xFF8B9CABu;}
bool recv_s3(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t recv_r4(uint32_t x){return (x<<3)^0xFF302712u;}
bool recv_s4(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t recv_r5(uint32_t x){return (x<<3)^0xFCB8AF9Du;}
bool recv_s5(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t recv_r6(uint32_t x){return (x<<3)^0xFC213604u;}
bool recv_s6(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t recv_r7(uint32_t x){return (x<<3)^0xFDA9BE8Fu;}
bool recv_s7(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t recv_r8(uint32_t x){return (x<<3)^0xFD564176u;}
bool recv_s8(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t recv_r9(uint32_t x){return (x<<3)^0xFADEC9E1u;}
bool recv_s9(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t recv_r10(uint32_t x){return (x<<3)^0xFA475068u;}
bool recv_s10(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t recv_r11(uint32_t x){return (x<<3)^0xFBCFD8D3u;}
bool recv_s11(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t recv_r12(uint32_t x){return (x<<3)^0xFB74635Au;}
bool recv_s12(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t recv_r13(uint32_t x){return (x<<3)^0xF8FCEBC5u;}
bool recv_s13(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t recv_r14(uint32_t x){return (x<<3)^0xF865724Cu;}
bool recv_s14(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t recv_r15(uint32_t x){return (x<<3)^0xF8120537u;}
bool recv_s15(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t recv_r16(uint32_t x){return (x<<3)^0xF99A8DBEu;}
bool recv_s16(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t recv_r17(uint32_t x){return (x<<3)^0xF9031429u;}
bool recv_s17(uint32_t a,uint32_t b){return a!=0||b==0;}
}  // namespace runtime




























































}  // namespace helion
