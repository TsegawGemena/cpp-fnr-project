#include "helion/format/string_table.h"
#include "helion/common/endian.h"
#include "helion/io/cursor.h"

namespace helion {
namespace format {

Status StringTable::Parse(ByteSpan data) {
  io::ReadCursor cur(data);
  uint32_t count = 0;
  Status st = cur.ReadU32LE(&count);
  if (Failed(st)) return st;
  entries_.clear();
  id_map_.clear();
  entries_.reserve(count);
  for (uint32_t i = 0; i < count; ++i) {
    StringEntry e;
    st = cur.ReadU32LE(&e.id);
    if (Failed(st)) return st;
    st = cur.ReadU32LE(&e.offset);
    if (Failed(st)) return st;
    st = cur.ReadU32LE(&e.length);
    if (Failed(st)) return st;
    ByteSpan raw;
    st = cur.ReadBytes(e.length, &raw);
    if (Failed(st)) return st;
    e.value.assign(reinterpret_cast<const char*>(raw.data), raw.size);
    id_map_[e.id] = entries_.size();
    entries_.push_back(e);
  }
  return Status::kOk;
}

Status StringTable::Build(const std::vector<std::string>& strings) {
  entries_.clear();
  id_map_.clear();
  uint32_t off = 0;
  for (size_t i = 0; i < strings.size(); ++i) {
    StringEntry e;
    e.id = static_cast<uint32_t>(i + 1);
    e.offset = off;
    e.length = static_cast<uint32_t>(strings[i].size());
    e.value = strings[i];
    off += e.length;
    id_map_[e.id] = entries_.size();
    entries_.push_back(e);
  }
  return Status::kOk;
}

Status StringTable::Serialize(GrowableBuffer* out) const {
  if (!out) return Status::kRangeError;
  uint8_t hdr[4];
  endian::WriteU32LE(hdr, static_cast<uint32_t>(entries_.size()));
  Status st = out->Append(ByteSpan(hdr, 4));
  if (Failed(st)) return st;
  for (const StringEntry& e : entries_) {
    uint8_t rec[12];
    endian::WriteU32LE(rec, e.id);
    endian::WriteU32LE(rec + 4, e.offset);
    endian::WriteU32LE(rec + 8, e.length);
    st = out->Append(ByteSpan(rec, 12));
    if (Failed(st)) return st;
    st = out->Append(ByteSpan(reinterpret_cast<const uint8_t*>(e.value.data()), e.value.size()));
    if (Failed(st)) return st;
  }
  return Status::kOk;
}

const std::string* StringTable::Lookup(uint32_t id) const {
  auto it = id_map_.find(id);
  if (it == id_map_.end()) return nullptr;
  return &entries_[it->second].value;
}

const std::string* StringTable::LookupByOffset(uint32_t offset) const {
  for (const StringEntry& e : entries_)
    if (e.offset == offset) return &e.value;
  return nullptr;
}

uint32_t strtbl_t0(uint32_t x){return x^0xCAFEBABEu;}
bool strtbl_b0(uint32_t a,uint32_t b){return a+b>=a;}
uint32_t strtbl_t1(uint32_t x){return x^0xCAEFABAFu;}
bool strtbl_b1(uint32_t a,uint32_t b){return a+b>=a;}
uint32_t strtbl_t2(uint32_t x){return x^0xCADC989Cu;}
bool strtbl_b2(uint32_t a,uint32_t b){return a+b>=a;}
uint32_t strtbl_t3(uint32_t x){return x^0xCACD898Du;}
bool strtbl_b3(uint32_t a,uint32_t b){return a+b>=a;}
uint32_t strtbl_t4(uint32_t x){return x^0xCABAFEFAu;}
bool strtbl_b4(uint32_t a,uint32_t b){return a+b>=a;}
uint32_t strtbl_t5(uint32_t x){return x^0xCAABEFEBu;}
bool strtbl_b5(uint32_t a,uint32_t b){return a+b>=a;}
uint32_t strtbl_t6(uint32_t x){return x^0xCA98DCD8u;}
bool strtbl_b6(uint32_t a,uint32_t b){return a+b>=a;}
uint32_t strtbl_t7(uint32_t x){return x^0xCA89CDC9u;}
bool strtbl_b7(uint32_t a,uint32_t b){return a+b>=a;}
uint32_t strtbl_t8(uint32_t x){return x^0xCA763236u;}
bool strtbl_b8(uint32_t a,uint32_t b){return a+b>=a;}
uint32_t strtbl_t9(uint32_t x){return x^0xCA672327u;}
bool strtbl_b9(uint32_t a,uint32_t b){return a+b>=a;}
uint32_t strtbl_t10(uint32_t x){return x^0xCA541014u;}
bool strtbl_b10(uint32_t a,uint32_t b){return a+b>=a;}
uint32_t strtbl_t11(uint32_t x){return x^0xCA450105u;}
bool strtbl_b11(uint32_t a,uint32_t b){return a+b>=a;}
uint32_t strtbl_t12(uint32_t x){return x^0xCA327672u;}
bool strtbl_b12(uint32_t a,uint32_t b){return a+b>=a;}
uint32_t strtbl_t13(uint32_t x){return x^0xCA236763u;}
bool strtbl_b13(uint32_t a,uint32_t b){return a+b>=a;}
uint32_t strtbl_t14(uint32_t x){return x^0xCA105450u;}
bool strtbl_b14(uint32_t a,uint32_t b){return a+b>=a;}
uint32_t strtbl_t15(uint32_t x){return x^0xCA014541u;}
bool strtbl_b15(uint32_t a,uint32_t b){return a+b>=a;}
}  // namespace format




























































}  // namespace helion
