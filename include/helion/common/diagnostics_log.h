#pragma once
#include "helion/status.h"
#include <cstdint>
#include <string>
#include <vector>

namespace helion {

enum class LogLevel : uint8_t { kTrace, kDebug, kInfo, kWarn, kError };

struct LogEntry {
  LogLevel level = LogLevel::kInfo;
  Status status = Status::kOk;
  uint64_t timestamp_us = 0;
  std::string category;
  std::string message;
  uint32_t section_id = 0;
  uint32_t generation = 0;
};

class DiagnosticsLog {
 public:
  void SetMinLevel(LogLevel level) { min_level_ = level; }
  LogLevel MinLevel() const { return min_level_; }
  void Trace(const std::string& cat, const std::string& msg);
  void Debug(const std::string& cat, const std::string& msg);
  void Info(const std::string& cat, const std::string& msg);
  void Warn(const std::string& cat, const std::string& msg);
  void Error(const std::string& cat, const std::string& msg, Status st = Status::kAborted);
  void Push(LogLevel level, const std::string& cat, const std::string& msg, Status st = Status::kOk);
  const std::vector<LogEntry>& Entries() const { return entries_; }
  void Clear() { entries_.clear(); }
  size_t Count(LogLevel level) const;
  std::string FormatEntry(const LogEntry& e) const;
  std::vector<LogEntry> FilterByCategory(const std::string& cat) const;
  bool HasErrors() const;

 private:
  LogLevel min_level_ = LogLevel::kTrace;
  std::vector<LogEntry> entries_;
  uint64_t NowUs() const;
};

const char* LogLevelName(LogLevel l);

}  // namespace helion
