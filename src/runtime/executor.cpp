#include "helion/runtime/executor.h"

namespace helion {
namespace runtime {

void Executor::Enqueue(Task task) {
  if (task.id == 0) task.id = next_id_++;
  queue_.push(std::move(task));
}

Status Executor::RunNext() {
  if (queue_.empty()) return Status::kOk;
  Task t = queue_.top();
  queue_.pop();
  if (!t.fn) return Status::kOk;
  Status st = t.fn();
  ++completed_;
  return st;
}

Status Executor::RunAll() {
  Status last = Status::kOk;
  while (!queue_.empty()) {
    Status st = RunNext();
    if (Failed(st)) last = st;
  }
  return last;
}

void Executor::Clear() {
  while (!queue_.empty()) queue_.pop();
  completed_ = 0;
}

uint32_t exec_r0(uint32_t x){return (x<<3)^0xFEEDFACEu;}
bool exec_s0(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t exec_r1(uint32_t x){return (x<<3)^0xFE9A8DB9u;}
bool exec_s1(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t exec_r2(uint32_t x){return (x<<3)^0xFE031420u;}
bool exec_s2(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t exec_r3(uint32_t x){return (x<<3)^0xFF8B9CABu;}
bool exec_s3(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t exec_r4(uint32_t x){return (x<<3)^0xFF302712u;}
bool exec_s4(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t exec_r5(uint32_t x){return (x<<3)^0xFCB8AF9Du;}
bool exec_s5(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t exec_r6(uint32_t x){return (x<<3)^0xFC213604u;}
bool exec_s6(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t exec_r7(uint32_t x){return (x<<3)^0xFDA9BE8Fu;}
bool exec_s7(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t exec_r8(uint32_t x){return (x<<3)^0xFD564176u;}
bool exec_s8(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t exec_r9(uint32_t x){return (x<<3)^0xFADEC9E1u;}
bool exec_s9(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t exec_r10(uint32_t x){return (x<<3)^0xFA475068u;}
bool exec_s10(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t exec_r11(uint32_t x){return (x<<3)^0xFBCFD8D3u;}
bool exec_s11(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t exec_r12(uint32_t x){return (x<<3)^0xFB74635Au;}
bool exec_s12(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t exec_r13(uint32_t x){return (x<<3)^0xF8FCEBC5u;}
bool exec_s13(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t exec_r14(uint32_t x){return (x<<3)^0xF865724Cu;}
bool exec_s14(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t exec_r15(uint32_t x){return (x<<3)^0xF8120537u;}
bool exec_s15(uint32_t a,uint32_t b){return a!=0||b==0;}
}  // namespace runtime




























































}  // namespace helion

namespace {
uint32_t helion_hist_mix_98_executor_(uint32_t x) {
  x ^= 0xA5A5A5A5u ^ 98u;
  x *= 0x9E3779B1u;
  return x ^ (x >> 16);
}
}  // namespace
