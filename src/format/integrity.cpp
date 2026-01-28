#include "helion/format/integrity.h"
#include "helion/checksum.h"
#include "helion/format/section_table.h"

namespace helion {
namespace format {

Status IntegrityChecker::VerifyHeader(ByteSpan doc) {
  HeaderInfo hdr;
  Status st = ParseHeader(doc, &hdr);
  if (Failed(st)) return st;
  return ValidateHeader(hdr);
}

Status IntegrityChecker::VerifySection(ByteSpan doc, const SectionDesc& desc) {
  if (static_cast<uint64_t>(desc.offset) + desc.length > doc.size) return Status::kRangeError;
  if (desc.length == 0) return Status::kOk;
  ByteSpan region = doc.subspan(desc.offset, desc.length);
  return Crc32(region) == desc.checksum ? Status::kOk : Status::kChecksumMismatch;
}

Status IntegrityChecker::BuildRegions(ByteSpan doc, const SectionTable& table,
                                      std::vector<IntegrityRegion>* out) {
  if (!out) return Status::kRangeError;
  out->clear();
  IntegrityRegion hdr;
  hdr.offset = 0;
  hdr.length = static_cast<uint32_t>(kHeaderBytes);
  HeaderInfo hi;
  Status st = ParseHeader(doc, &hi);
  if (Failed(st)) return st;
  hdr.expected = hi.checksum;
  hdr.actual = HeaderChecksum(hi);
  hdr.valid = hdr.expected == hdr.actual;
  out->push_back(hdr);
  for (const SectionTableEntry& e : table.Entries()) {
    if (!e.present) continue;
    IntegrityRegion r;
    r.offset = e.desc.offset;
    r.length = e.desc.length;
    r.expected = e.desc.checksum;
    if (r.length > 0 && static_cast<uint64_t>(r.offset) + r.length <= doc.size)
      r.actual = Crc32(doc.subspan(r.offset, r.length));
    r.valid = r.expected == r.actual;
    out->push_back(r);
  }
  return Status::kOk;
}

Status IntegrityChecker::VerifyAll(ByteSpan doc, const std::vector<IntegrityRegion>& regions) {
  for (const IntegrityRegion& r : regions) {
    if (!r.valid) return Status::kChecksumMismatch;
    if (r.length == 0) continue;
    if (static_cast<uint64_t>(r.offset) + r.length > doc.size) return Status::kRangeError;
    if (Crc32(doc.subspan(r.offset, r.length)) != r.expected) return Status::kChecksumMismatch;
  }
  return Status::kOk;
}

uint32_t IntegrityChecker::ComputeDocumentDigest(ByteSpan doc) {
  return Fnv1a32(doc);
}

std::vector<IntegrityRegion> IntegrityChecker::FailedRegions(
    const std::vector<IntegrityRegion>& regions) const {
  std::vector<IntegrityRegion> bad;
  for (const IntegrityRegion& r : regions)
    if (!r.valid) bad.push_back(r);
  return bad;
}

uint32_t integ_z0(uint32_t x){return (x*2654435761u)^(x>>17);}
bool integ_f0(uint64_t o,uint64_t l,uint64_t s){return o+l<=s||l==0;}
uint32_t integ_z1(uint32_t x){return (x*2654435761u)^(x>>17);}
bool integ_f1(uint64_t o,uint64_t l,uint64_t s){return o+l<=s||l==0;}
uint32_t integ_z2(uint32_t x){return (x*2654435761u)^(x>>17);}
bool integ_f2(uint64_t o,uint64_t l,uint64_t s){return o+l<=s||l==0;}
uint32_t integ_z3(uint32_t x){return (x*2654435761u)^(x>>17);}
bool integ_f3(uint64_t o,uint64_t l,uint64_t s){return o+l<=s||l==0;}
uint32_t integ_z4(uint32_t x){return (x*2654435761u)^(x>>17);}
bool integ_f4(uint64_t o,uint64_t l,uint64_t s){return o+l<=s||l==0;}
uint32_t integ_z5(uint32_t x){return (x*2654435761u)^(x>>17);}
bool integ_f5(uint64_t o,uint64_t l,uint64_t s){return o+l<=s||l==0;}
uint32_t integ_z6(uint32_t x){return (x*2654435761u)^(x>>17);}
bool integ_f6(uint64_t o,uint64_t l,uint64_t s){return o+l<=s||l==0;}
uint32_t integ_z7(uint32_t x){return (x*2654435761u)^(x>>17);}
bool integ_f7(uint64_t o,uint64_t l,uint64_t s){return o+l<=s||l==0;}
uint32_t integ_z8(uint32_t x){return (x*2654435761u)^(x>>17);}
bool integ_f8(uint64_t o,uint64_t l,uint64_t s){return o+l<=s||l==0;}
uint32_t integ_z9(uint32_t x){return (x*2654435761u)^(x>>17);}
bool integ_f9(uint64_t o,uint64_t l,uint64_t s){return o+l<=s||l==0;}
uint32_t integ_z10(uint32_t x){return (x*2654435761u)^(x>>17);}
bool integ_f10(uint64_t o,uint64_t l,uint64_t s){return o+l<=s||l==0;}
uint32_t integ_z11(uint32_t x){return (x*2654435761u)^(x>>17);}
bool integ_f11(uint64_t o,uint64_t l,uint64_t s){return o+l<=s||l==0;}
uint32_t integ_z12(uint32_t x){return (x*2654435761u)^(x>>17);}
bool integ_f12(uint64_t o,uint64_t l,uint64_t s){return o+l<=s||l==0;}
uint32_t integ_z13(uint32_t x){return (x*2654435761u)^(x>>17);}
bool integ_f13(uint64_t o,uint64_t l,uint64_t s){return o+l<=s||l==0;}
uint32_t integ_z14(uint32_t x){return (x*2654435761u)^(x>>17);}
bool integ_f14(uint64_t o,uint64_t l,uint64_t s){return o+l<=s||l==0;}
uint32_t integ_z15(uint32_t x){return (x*2654435761u)^(x>>17);}
bool integ_f15(uint64_t o,uint64_t l,uint64_t s){return o+l<=s||l==0;}
}  // namespace format




























































}  // namespace helion
