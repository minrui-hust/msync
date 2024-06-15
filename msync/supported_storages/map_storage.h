#pragma once

#include <memory>

#include "../storage.h"
#include "../traits.h"

namespace msync {

template <typename _Msg, typename _Alloc>
using TimeMsgMap = std::map<Time, _Msg, std::less<Time>, _Alloc>;

template <typename _Msg,
          typename _Alloc = std::allocator<std::pair<const Time, _Msg>>>
struct MapStorage;

template <typename _Msg, typename _Alloc>
struct StorageTraits<MapStorage<_Msg, _Alloc>> {
  using MsgType = _Msg;
  using ConstIter = typename TimeMsgMap<_Msg, _Alloc>::const_iterator;
};

template <typename _Msg, typename _Alloc>
struct MapStorage : public StorageBase<MapStorage<_Msg, _Alloc>> {
  using Base = StorageBase<MapStorage<_Msg, _Alloc>>;
  using MsgType = typename StorageTraits<MapStorage>::MsgType;
  using ConstIter = typename StorageTraits<MapStorage>::ConstIter;

  using Base::history_win_;

  MapStorage(const Time history_win) : Base(history_win) {}

  // interface implementations

  bool pushImpl(const int64_t &time, const MsgType &msg) {
    if (!stamp2msg_.empty() && time <= stamp2msg_.rbegin()->first) {
      return false;
    }

    stamp2msg_.insert(std::make_pair(time, msg));

    // check if we should remove the old ones
    if (stamp2msg_.begin()->first < time - history_win_) {
      stamp2msg_.erase(stamp2msg_.begin());
    }

    return true;
  }

  size_t sizeImpl() const { return stamp2msg_.size(); }

  bool emptyImpl() const { return stamp2msg_.empty(); }

  ConstIter beginImpl() const { return stamp2msg_.begin(); }

  ConstIter endImpl() const { return stamp2msg_.end(); }

  ConstIter findImpl(const Time time) const { return stamp2msg_.find(time); }

  ConstIter findPreImpl(const Time time) const {
    auto upper = stamp2msg_.upper_bound(time);
    return upper != stamp2msg_.begin() ? std::prev(upper) : stamp2msg_.end();
  }

  ConstIter findSucImpl(const Time time) const {
    return stamp2msg_.upper_bound(time);
  }

  std::pair<Time, MsgType> frontImpl() const { return *stamp2msg_.begin(); }

  std::pair<Time, MsgType> backImpl() const { return *stamp2msg_.rbegin(); }

protected:
  TimeMsgMap<_Msg, _Alloc> stamp2msg_;
};

} // namespace msync
