#include "PllSplits.hpp"
#include "PllTree.hpp"
#include "../enums.hpp"

size_t PllSplit::tip_count = 0;
pll_split_base_t PllSplit::bitmask_for_unused_bits = 0;

size_t PllSplit::popcount() const{
  assert(splitValid());
  size_t popcount = 0;
  size_t split_len = PllSplit::getSplitLen();
  for(size_t i = 0; i < split_len; ++i){
    popcount += basePopcount(_split[i]);
  }
  return popcount;
}

bool operator == (const PllSplit& p1, const PllSplit& p2) {
  size_t split_len = PllSplit::getSplitLen();
  for(unsigned i = 0; i < split_len; ++i) {
    if(p1._split[i] != p2._split[i]) {
      return false;
    }
  }
  return true;
}
bool operator < (const PllSplit&p1, const PllSplit& p2) {
  size_t split_len = PllSplit::getSplitLen();
  for(unsigned i = 0; i < split_len; ++i) {
    if(p1._split[i] != p2._split[i]) {
      return (p1._split[i] < p2._split[i]);
    }
  }
  return false;
}
//TODO, we should kill this once we are certain that we don't need it
uint32_t PllSplit::bitExtract(size_t bit_index) const {
  assert(splitValid());
  //assert(bit_index < PllSplit::getTipCount()); //TODO reenable this assertion if the test for validity is deprecated
  pll_split_base_t split_part = _split[computeMajorIndex(bit_index)];
  return (split_part & (1u << computeMinorIndex(bit_index))) >> computeMinorIndex(bit_index);
}
//TODO inline and move to header
size_t PllSplit::partitionSizeOf (partition_t block) const {
  assert(splitValid());
  return block ? this->popcount() : PllSplit::getTipCount() - this->popcount();
}

//TODO think about rewriting this/other to A/B?
size_t PllSplit::intersectionSize(const PllSplit& other, 
                                  partition_t partition_this, partition_t partition_other) const {
  assert(splitValid());
  assert(other.splitValid());
  size_t split_len = PllSplit::getSplitLen();
  pll_split_base_t this_mask = partition_this ? 0 : ~0u; //TODO explanatory text
  pll_split_base_t other_mask = partition_other ? 0 : ~0u;
  size_t count = 0;

  for (size_t i = 0; i < split_len - 1; ++i){
    count += basePopcount((_split[i] ^ this_mask) & (other()[i] ^ other_mask));
  }
  count += basePopcount((_split[split_len - 1] ^ this_mask) & (other()[split_len - 1] ^ other_mask)
                        & PllSplit::bitmask_for_unused_bits);
  return count;
}


//TODO @Robin might wanna do a speedtest, or find another implementation with registers
size_t PllSplit::basePopcount(pll_split_base_t val) const {
  return std::bitset<32>(val).count();
}

bool PllSplit::splitValid() const {
  return (_split != nullptr) && !(_split[PllSplit::getSplitLen() - 1] 
                                  & ~PllSplit::bitmask_for_unused_bits) && _split[0] & 1u;
}


//TODO find out if preallocation can be done.
PllSplitList::PllSplitList(const PllTree &tree) {
  assert(PllSplit::getTipCount() == tree.getTipCount());
  pll_split_t* tmp_splits = pllmod_utree_split_create(tree.tree()->vroot, tree.getTipCount(), nullptr);
  //_splits.reserve(tree.tree()->tip_count - 3);
  for (size_t i = 0; i < tree.getTipCount() - 3; ++i) {
    _splits.emplace_back(PllSplit(tmp_splits[i]));
  }
  free(tmp_splits);
}
//TODO find out if preallocation can be done
PllSplitList::PllSplitList(const std::vector<PllSplit> &splits) {
  size_t split_len = PllSplit::getSplitLen();
  if(splits.size() > 0){
    pll_split_t split_pointer = (pll_split_t) calloc(splits.size()* split_len, sizeof(pll_split_base_t));
    for (size_t i=0; i<splits.size(); ++i) {
      memcpy(split_pointer + i* split_len, splits[i](), split_len * sizeof(pll_split_base_t));
      _splits.emplace_back(PllSplit(split_pointer + i*split_len));
    }
  }
}

PllSplitList::~PllSplitList() {
  if (!_splits.empty()) { free(_splits[0]()); }
}
bool operator == (const PllSplitList& p1, const PllSplitList& p2) {
  const std::vector<PllSplit>& splits1 = p1.getSplits();
  const std::vector<PllSplit>& splits2 = p2.getSplits();
  if(splits1.size() != splits2.size()) return false;
  for(size_t i = 0; i < splits1.size(); ++i){
    if (!(splits1[i] == splits2[i])) return false;
  }
  return true;
}
