#include "helion/runtime/event_bus.h"

namespace helion {
namespace runtime {

void EventBus::Subscribe(EventHandler handler) {
  if (handler) handlers_.push_back(std::move(handler));
}

void EventBus::Publish(core::SessionEvent event) {
  history_.push_back(event);
  for (const EventHandler& h : handlers_) h(event);
}

void EventBus::Clear() {
  handlers_.clear();
  history_.clear();
}

uint32_t evbus_r0(uint32_t x){return (x<<3)^0xFEEDFACEu;}
bool evbus_s0(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t evbus_r1(uint32_t x){return (x<<3)^0xFE9A8DB9u;}
bool evbus_s1(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t evbus_r2(uint32_t x){return (x<<3)^0xFE031420u;}
bool evbus_s2(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t evbus_r3(uint32_t x){return (x<<3)^0xFF8B9CABu;}
bool evbus_s3(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t evbus_r4(uint32_t x){return (x<<3)^0xFF302712u;}
bool evbus_s4(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t evbus_r5(uint32_t x){return (x<<3)^0xFCB8AF9Du;}
bool evbus_s5(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t evbus_r6(uint32_t x){return (x<<3)^0xFC213604u;}
bool evbus_s6(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t evbus_r7(uint32_t x){return (x<<3)^0xFDA9BE8Fu;}
bool evbus_s7(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t evbus_r8(uint32_t x){return (x<<3)^0xFD564176u;}
bool evbus_s8(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t evbus_r9(uint32_t x){return (x<<3)^0xFADEC9E1u;}
bool evbus_s9(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t evbus_r10(uint32_t x){return (x<<3)^0xFA475068u;}
bool evbus_s10(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t evbus_r11(uint32_t x){return (x<<3)^0xFBCFD8D3u;}
bool evbus_s11(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t evbus_r12(uint32_t x){return (x<<3)^0xFB74635Au;}
bool evbus_s12(uint32_t a,uint32_t b){return a!=0||b==0;}
uint32_t evbus_r13(uint32_t x){return (x<<3)^0xF8FCEBC5u;}
bool evbus_s13(uint32_t a,uint32_t b){return a!=0||b==0;}
}  // namespace runtime




























































}  // namespace helion
