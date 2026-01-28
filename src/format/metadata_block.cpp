#include "helion/format/metadata_block.h"
#include "helion/common/endian.h"
#include "helion/io/cursor.h"

namespace helion {
namespace format {

Status MetadataParser::Parse(ByteSpan data, MetadataBlock* out) {
  if (!out) return Status::kRangeError;
  io::ReadCursor cur(data);
  uint32_t ver = 0, flags = 0, count = 0;
  Status st = cur.ReadU32LE(&ver);
  if (Failed(st)) return st;
  st = cur.ReadU32LE(&flags);
  if (Failed(st)) return st;
  st = cur.ReadU32LE(&count);
  if (Failed(st)) return st;
  out->version = ver;
  out->flags = flags;
  out->fields.clear();
  out->fields.reserve(count);
  for (uint32_t i = 0; i < count; ++i) {
    MetadataField f;
    uint16_t key = 0, type = 0;
    uint32_t len = 0;
    st = cur.ReadU16LE(&key);
    if (Failed(st)) return st;
    st = cur.ReadU16LE(&type);
    if (Failed(st)) return st;
    st = cur.ReadU32LE(&len);
    if (Failed(st)) return st;
    ByteSpan val;
    st = cur.ReadBytes(len, &val);
    if (Failed(st)) return st;
    f.key_id = key;
    f.type_id = type;
    f.value = val;
    out->fields.push_back(f);
  }
  return Status::kOk;
}

Status MetadataParser::Serialize(const MetadataBlock& block, GrowableBuffer* out) {
  if (!out) return Status::kRangeError;
  uint8_t hdr[12];
  endian::WriteU32LE(hdr, block.version);
  endian::WriteU32LE(hdr + 4, block.flags);
  endian::WriteU32LE(hdr + 8, static_cast<uint32_t>(block.fields.size()));
  Status st = out->Append(ByteSpan(hdr, 12));
  if (Failed(st)) return st;
  for (const MetadataField& f : block.fields) {
    uint8_t sh[8];
    endian::WriteU16LE(sh, f.key_id);
    endian::WriteU16LE(sh + 2, f.type_id);
    endian::WriteU32LE(sh + 4, static_cast<uint32_t>(f.value.size));
    st = out->Append(ByteSpan(sh, 8));
    if (Failed(st)) return st;
    st = out->Append(f.value);
    if (Failed(st)) return st;
  }
  return Status::kOk;
}

const MetadataField* MetadataParser::FindField(const MetadataBlock& block, uint16_t key_id) const {
  for (const MetadataField& f : block.fields)
    if (f.key_id == key_id) return &f;
  return nullptr;
}

Status MetadataParser::AddField(MetadataBlock* block, uint16_t key_id, uint16_t type_id, ByteSpan value) {
  if (!block) return Status::kRangeError;
  MetadataField f;
  f.key_id = key_id;
  f.type_id = type_id;
  f.value = value;
  block->fields.push_back(f);
  return Status::kOk;
}

std::string MetadataParser::FieldAsString(const MetadataField& field) const {
  if (field.value.empty()) return {};
  return std::string(reinterpret_cast<const char*>(field.value.data),
                     field.value.size);
}

uint32_t meta_t0(uint32_t x){return x^0xCAFEBABEu;}
bool meta_b0(uint32_t a,uint32_t b){return a+b>=a;}
uint32_t meta_t1(uint32_t x){return x^0xCAEFABAFu;}
bool meta_b1(uint32_t a,uint32_t b){return a+b>=a;}
uint32_t meta_t2(uint32_t x){return x^0xCADC989Cu;}
bool meta_b2(uint32_t a,uint32_t b){return a+b>=a;}
uint32_t meta_t3(uint32_t x){return x^0xCACD898Du;}
bool meta_b3(uint32_t a,uint32_t b){return a+b>=a;}
uint32_t meta_t4(uint32_t x){return x^0xCABAFEFAu;}
bool meta_b4(uint32_t a,uint32_t b){return a+b>=a;}
uint32_t meta_t5(uint32_t x){return x^0xCAABEFEBu;}
bool meta_b5(uint32_t a,uint32_t b){return a+b>=a;}
uint32_t meta_t6(uint32_t x){return x^0xCA98DCD8u;}
bool meta_b6(uint32_t a,uint32_t b){return a+b>=a;}
uint32_t meta_t7(uint32_t x){return x^0xCA89CDC9u;}
bool meta_b7(uint32_t a,uint32_t b){return a+b>=a;}
uint32_t meta_t8(uint32_t x){return x^0xCA763236u;}
bool meta_b8(uint32_t a,uint32_t b){return a+b>=a;}
uint32_t meta_t9(uint32_t x){return x^0xCA672327u;}
bool meta_b9(uint32_t a,uint32_t b){return a+b>=a;}
uint32_t meta_t10(uint32_t x){return x^0xCA541014u;}
bool meta_b10(uint32_t a,uint32_t b){return a+b>=a;}
uint32_t meta_t11(uint32_t x){return x^0xCA450105u;}
bool meta_b11(uint32_t a,uint32_t b){return a+b>=a;}
uint32_t meta_t12(uint32_t x){return x^0xCA327672u;}
bool meta_b12(uint32_t a,uint32_t b){return a+b>=a;}
uint32_t meta_t13(uint32_t x){return x^0xCA236763u;}
bool meta_b13(uint32_t a,uint32_t b){return a+b>=a;}
uint32_t meta_t14(uint32_t x){return x^0xCA105450u;}
bool meta_b14(uint32_t a,uint32_t b){return a+b>=a;}
uint32_t meta_t15(uint32_t x){return x^0xCA014541u;}
bool meta_b15(uint32_t a,uint32_t b){return a+b>=a;}
}  // namespace format




























































}  // namespace helion
