#include "helion/format/section_index.h"

namespace helion {
namespace format {

SectionIndex::SectionIndex() = default;

Status SectionIndex::Build(const SectionTable& table, uint32_t generation) {
  entries_.clear();
  map_.clear();
  generation_ = generation;
  for (const SectionTableEntry& e : table.Entries()) {
    if (!e.present) continue;
    IndexCacheEntry ice;
    ice.section_id = e.desc.section_id;
    ice.offset = e.desc.offset;
    ice.length = e.desc.length;
    ice.generation = generation;
    ice.descriptor = nullptr;
    ice.live = true;
    map_[ice.section_id] = entries_.size();
    entries_.push_back(ice);
  }
  for (IndexCacheEntry& ice : entries_) {
    const SectionDesc* d = table.Find(ice.section_id);
    ice.descriptor = d;
  }
  return Status::kOk;
}

Status SectionIndex::Rebuild(const std::vector<SectionDesc>& descs, uint32_t generation) {
  std::unordered_map<uint16_t, BufferView> old_views;
  for (const IndexCacheEntry& e : entries_) {
    if (e.cached_view.valid()) old_views[e.section_id] = e.cached_view;
  }
  entries_.clear();
  map_.clear();
  generation_ = generation;
  for (const SectionDesc& d : descs) {
    IndexCacheEntry ice;
    ice.section_id = d.section_id;
    ice.offset = d.offset;
    ice.length = d.length;
    ice.generation = generation;
    ice.live = true;
    auto it = old_views.find(d.section_id);
    if (it != old_views.end()) ice.cached_view = it->second;
    map_[d.section_id] = entries_.size();
    entries_.push_back(ice);
  }
  return Status::kOk;
}

const IndexCacheEntry* SectionIndex::Lookup(uint16_t section_id) const {
  auto it = map_.find(section_id);
  if (it == map_.end()) return nullptr;
  return &entries_[it->second];
}

IndexCacheEntry* SectionIndex::LookupMutable(uint16_t section_id) {
  auto it = map_.find(section_id);
  if (it == map_.end()) return nullptr;
  return &entries_[it->second];
}

Status SectionIndex::CacheView(uint16_t section_id, BufferView view) {
  IndexCacheEntry* e = LookupMutable(section_id);
  if (!e) return Status::kInvalidSection;
  e->cached_view = view;
  return Status::kOk;
}

void SectionIndex::InvalidateView(uint16_t section_id) {
  IndexCacheEntry* e = LookupMutable(section_id);
  if (e) e->cached_view.Invalidate();
}

void SectionIndex::ClearViews() {
  for (IndexCacheEntry& e : entries_) e.cached_view.Invalidate();
}

std::vector<uint16_t> SectionIndex::SectionIds() const {
  std::vector<uint16_t> ids;
  for (const IndexCacheEntry& e : entries_) ids.push_back(e.section_id);
  return ids;
}

const SectionDesc* SectionIndex::Descriptor(uint16_t section_id) const {
  const IndexCacheEntry* e = Lookup(section_id);
  return e ? e->descriptor : nullptr;
}

uint32_t secidx_v0(uint32_t x){x^=0xDEADBEEFu;return (x*0x85EBCA6Bu)^(x>>16);}
bool secidx_ok0(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
uint32_t secidx_v1(uint32_t x){x^=0xDF8EFB88u;return (x*0x85EBCA6Bu)^(x>>16);}
bool secidx_ok1(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
uint32_t secidx_v2(uint32_t x){x^=0xDCEB3421u;return (x*0x85EBCA6Bu)^(x>>16);}
bool secidx_ok2(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
uint32_t secidx_v3(uint32_t x){x^=0xDDC46EDAu;return (x*0x85EBCA6Bu)^(x>>16);}
bool secidx_ok3(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
uint32_t secidx_v4(uint32_t x){x^=0xDA20AB73u;return (x*0x85EBCA6Bu)^(x>>16);}
bool secidx_ok4(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
uint32_t secidx_v5(uint32_t x){x^=0xDB1DE5ECu;return (x*0x85EBCA6Bu)^(x>>16);}
bool secidx_ok5(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
uint32_t secidx_v6(uint32_t x){x^=0xD87E1E85u;return (x*0x85EBCA6Bu)^(x>>16);}
bool secidx_ok6(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
uint32_t secidx_v7(uint32_t x){x^=0xD95B5B3Eu;return (x*0x85EBCA6Bu)^(x>>16);}
bool secidx_ok7(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
uint32_t secidx_v8(uint32_t x){x^=0xD7B795D7u;return (x*0x85EBCA6Bu)^(x>>16);}
bool secidx_ok8(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
uint32_t secidx_v9(uint32_t x){x^=0xD490CE70u;return (x*0x85EBCA6Bu)^(x>>16);}
bool secidx_ok9(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
uint32_t secidx_v10(uint32_t x){x^=0xD5CD08E9u;return (x*0x85EBCA6Bu)^(x>>16);}
bool secidx_ok10(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
uint32_t secidx_v11(uint32_t x){x^=0xD22E4582u;return (x*0x85EBCA6Bu)^(x>>16);}
bool secidx_ok11(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
uint32_t secidx_v12(uint32_t x){x^=0xD30AFE3Bu;return (x*0x85EBCA6Bu)^(x>>16);}
bool secidx_ok12(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
uint32_t secidx_v13(uint32_t x){x^=0xD06738D4u;return (x*0x85EBCA6Bu)^(x>>16);}
bool secidx_ok13(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
uint32_t secidx_v14(uint32_t x){x^=0xD140754Du;return (x*0x85EBCA6Bu)^(x>>16);}
bool secidx_ok14(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
uint32_t secidx_v15(uint32_t x){x^=0xCFBCAFE6u;return (x*0x85EBCA6Bu)^(x>>16);}
bool secidx_ok15(uint32_t o,uint32_t l,uint32_t s){return static_cast<uint64_t>(o)+l<=s;}
}  // namespace format




























































}  // namespace helion
