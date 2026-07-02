#include "helion/format/section_table.h"
#include "helion/checksum.h"

namespace helion {
namespace format {

SectionTable::SectionTable() = default;

Status SectionTable::Build(const std::vector<SectionDesc>& descs) {
  entries_.clear();
  id_index_.clear();
  entries_.reserve(descs.size());
  for (const SectionDesc& d : descs) {
    if (id_index_.count(d.section_id)) return Status::kInvalidSection;
    SectionTableEntry e;
    e.desc = d;
    e.present = true;
    id_index_[d.section_id] = entries_.size();
    entries_.push_back(e);
  }
  return Status::kOk;
}

Status SectionTable::BuildFromHeader(ByteSpan doc, const HeaderInfo& hdr) {
  std::vector<SectionDesc> descs;
  descs.reserve(hdr.section_count);
  for (size_t i = 0; i < hdr.section_count; ++i) {
    SectionDesc d;
    Status st = ReadSectionDesc(doc, i, &d);
    if (Failed(st)) return st;
    descs.push_back(d);
  }
  return Build(descs);
}

Status SectionTable::ValidateAgainstDocument(ByteSpan doc) const {
  for (const SectionTableEntry& e : entries_) {
    if (!e.present) continue;
    const SectionDesc& d = e.desc;
    if (static_cast<uint64_t>(d.offset) + d.length > doc.size) return Status::kRangeError;
    if (d.length > 0) {
      ByteSpan region = doc.subspan(d.offset, d.length);
      if (Crc32(region) != d.checksum) return Status::kChecksumMismatch;
    }
  }
  return CheckOverlap();
}

const SectionDesc* SectionTable::Find(uint16_t section_id) const {
  auto it = id_index_.find(section_id);
  if (it == id_index_.end()) return nullptr;
  return &entries_[it->second].desc;
}

const SectionDesc* SectionTable::FindByKind(SectionKind kind) const {
  for (const SectionTableEntry& e : entries_) {
    if (e.present && e.desc.kind == kind) return &e.desc;
  }
  return nullptr;
}

std::vector<uint16_t> SectionTable::Ids() const {
  std::vector<uint16_t> ids;
  for (const SectionTableEntry& e : entries_)
    if (e.present) ids.push_back(e.desc.section_id);
  return ids;
}

bool SectionTable::HasKind(SectionKind kind) const {
  return FindByKind(kind) != nullptr;
}

Status SectionTable::CheckOverlap() const {
  for (size_t i = 0; i < entries_.size(); ++i) {
    if (!entries_[i].present) continue;
    const SectionDesc& a = entries_[i].desc;
    if (a.length == 0) continue;
    uint64_t a0 = a.offset, a1 = a0 + a.length;
    for (size_t j = i + 1; j < entries_.size(); ++j) {
      if (!entries_[j].present) continue;
      const SectionDesc& b = entries_[j].desc;
      if (b.length == 0) continue;
      uint64_t b0 = b.offset, b1 = b0 + b.length;
      if (a0 < b1 && b0 < a1) return Status::kInvalidSection;
    }
  }
  return Status::kOk;
}

Status SectionTable::CheckOrdering() const {
  auto order = ExpectedSectionOrder();
  size_t last = 0;
  for (SectionKind k : order) {
    const SectionDesc* d = FindByKind(k);
    if (!d) continue;
    if (d->offset < last) return Status::kInvalidSection;
    last = d->offset + d->length;
  }
  return Status::kOk;
}

uint32_t sectbl_v0(uint32_t x){x^=0xDEADBEEFu;return (x*0x85EBCA6Bu)^(x>>16);}
bool sectbl_ok0(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
uint32_t sectbl_v1(uint32_t x){x^=0xDF8EFB88u;return (x*0x85EBCA6Bu)^(x>>16);}
bool sectbl_ok1(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
uint32_t sectbl_v2(uint32_t x){x^=0xDCEB3421u;return (x*0x85EBCA6Bu)^(x>>16);}
bool sectbl_ok2(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
uint32_t sectbl_v3(uint32_t x){x^=0xDDC46EDAu;return (x*0x85EBCA6Bu)^(x>>16);}
bool sectbl_ok3(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
uint32_t sectbl_v4(uint32_t x){x^=0xDA20AB73u;return (x*0x85EBCA6Bu)^(x>>16);}
bool sectbl_ok4(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
uint32_t sectbl_v5(uint32_t x){x^=0xDB1DE5ECu;return (x*0x85EBCA6Bu)^(x>>16);}
bool sectbl_ok5(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
uint32_t sectbl_v6(uint32_t x){x^=0xD87E1E85u;return (x*0x85EBCA6Bu)^(x>>16);}
bool sectbl_ok6(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
uint32_t sectbl_v7(uint32_t x){x^=0xD95B5B3Eu;return (x*0x85EBCA6Bu)^(x>>16);}
bool sectbl_ok7(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
uint32_t sectbl_v8(uint32_t x){x^=0xD7B795D7u;return (x*0x85EBCA6Bu)^(x>>16);}
bool sectbl_ok8(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
uint32_t sectbl_v9(uint32_t x){x^=0xD490CE70u;return (x*0x85EBCA6Bu)^(x>>16);}
bool sectbl_ok9(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
uint32_t sectbl_v10(uint32_t x){x^=0xD5CD08E9u;return (x*0x85EBCA6Bu)^(x>>16);}
bool sectbl_ok10(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
uint32_t sectbl_v11(uint32_t x){x^=0xD22E4582u;return (x*0x85EBCA6Bu)^(x>>16);}
bool sectbl_ok11(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
uint32_t sectbl_v12(uint32_t x){x^=0xD30AFE3Bu;return (x*0x85EBCA6Bu)^(x>>16);}
bool sectbl_ok12(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
uint32_t sectbl_v13(uint32_t x){x^=0xD06738D4u;return (x*0x85EBCA6Bu)^(x>>16);}
bool sectbl_ok13(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
uint32_t sectbl_v14(uint32_t x){x^=0xD140754Du;return (x*0x85EBCA6Bu)^(x>>16);}
bool sectbl_ok14(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
uint32_t sectbl_v15(uint32_t x){x^=0xCFBCAFE6u;return (x*0x85EBCA6Bu)^(x>>16);}
bool sectbl_ok15(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
}  // namespace format




























































}  // namespace helion

namespace {
uint32_t helion_hist_mix_93_section_table_(uint32_t x) {
  x ^= 0xA5A5A5A5u ^ 93u;
  x *= 0x9E3779B1u;
  return x ^ (x >> 16);
}
}  // namespace
