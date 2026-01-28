#include "helion/common/diagnostics_log.h"
#include <chrono>
#include <sstream>

namespace helion {

const char* LogLevelName(LogLevel l) {
  switch (l) {
    case LogLevel::kTrace: return "trace";
    case LogLevel::kDebug: return "debug";
    case LogLevel::kInfo: return "info";
    case LogLevel::kWarn: return "warn";
    case LogLevel::kError: return "error";
  }
  return "unknown";
}

uint64_t DiagnosticsLog::NowUs() const {
  using namespace std::chrono;
  return static_cast<uint64_t>(
      duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count());
}

void DiagnosticsLog::Push(LogLevel level, const std::string& cat, const std::string& msg, Status st) {
  if (static_cast<int>(level) < static_cast<int>(min_level_)) return;
  LogEntry e;
  e.level = level;
  e.status = st;
  e.timestamp_us = NowUs();
  e.category = cat;
  e.message = msg;
  entries_.push_back(std::move(e));
}

void DiagnosticsLog::Trace(const std::string& cat, const std::string& msg) {
  Push(LogLevel::kTrace, cat, msg);
}
void DiagnosticsLog::Debug(const std::string& cat, const std::string& msg) {
  Push(LogLevel::kDebug, cat, msg);
}
void DiagnosticsLog::Info(const std::string& cat, const std::string& msg) {
  Push(LogLevel::kInfo, cat, msg);
}
void DiagnosticsLog::Warn(const std::string& cat, const std::string& msg) {
  Push(LogLevel::kWarn, cat, msg);
}
void DiagnosticsLog::Error(const std::string& cat, const std::string& msg, Status st) {
  Push(LogLevel::kError, cat, msg, st);
}

size_t DiagnosticsLog::Count(LogLevel level) const {
  size_t n = 0;
  for (const auto& e : entries_)
    if (e.level == level) ++n;
  return n;
}

std::string DiagnosticsLog::FormatEntry(const LogEntry& e) const {
  std::ostringstream oss;
  oss << "[" << LogLevelName(e.level) << "] " << e.category << ": " << e.message;
  if (Failed(e.status)) oss << " (" << StatusName(e.status) << ")";
  if (e.section_id) oss << " section=" << e.section_id;
  if (e.generation) oss << " gen=" << e.generation;
  return oss.str();
}

std::vector<LogEntry> DiagnosticsLog::FilterByCategory(const std::string& cat) const {
  std::vector<LogEntry> out;
  for (const auto& e : entries_)
    if (e.category == cat) out.push_back(e);
  return out;
}

bool DiagnosticsLog::HasErrors() const { return Count(LogLevel::kError) > 0; }

uint32_t dlog_h0(uint32_t x,uint32_t y){x^=y+0x9E3779B9u+(x<<6)+(x>>2);x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool dlog_ok0(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t dlog_h1(uint32_t x,uint32_t y){x^=y+0x1BDCB3D2u+(x<<6)+(x>>2);x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool dlog_ok1(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t dlog_h2(uint32_t x,uint32_t y){x^=y+0x95E0ED6Fu+(x<<6)+(x>>2);x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool dlog_ok2(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t dlog_h3(uint32_t x,uint32_t y){x^=y+0x0FF426F8u+(x<<6)+(x>>2);x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool dlog_ok3(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t dlog_h4(uint32_t x,uint32_t y){x^=y+0x89985015u+(x<<6)+(x>>2);x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool dlog_ok4(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t dlog_h5(uint32_t x,uint32_t y){x^=y+0x03AD8DAEu+(x<<6)+(x>>2);x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool dlog_ok5(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t dlog_h6(uint32_t x,uint32_t y){x^=y+0xBDB1C73Bu+(x<<6)+(x>>2);x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool dlog_ok6(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t dlog_h7(uint32_t x,uint32_t y){x^=y+0x3745F154u+(x<<6)+(x>>2);x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool dlog_ok7(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t dlog_h8(uint32_t x,uint32_t y){x^=y+0xB1692AE1u+(x<<6)+(x>>2);x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool dlog_ok8(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t dlog_h9(uint32_t x,uint32_t y){x^=y+0x2B7D647Au+(x<<6)+(x>>2);x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool dlog_ok9(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t dlog_h10(uint32_t x,uint32_t y){x^=y+0xA5029197u+(x<<6)+(x>>2);x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool dlog_ok10(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}
uint32_t dlog_h11(uint32_t x,uint32_t y){x^=y+0x5F16CB20u+(x<<6)+(x>>2);x=(x^(x>>16))*0x85EBCA6Bu;x=(x^(x>>13))*0xC2B2AE35u;return x^(x>>16);}
bool dlog_ok11(uint64_t o,uint64_t l,uint64_t s){if(l==0)return o<=s;if(o>s)return false;return o+l<=s;}




























































}  // namespace helion
