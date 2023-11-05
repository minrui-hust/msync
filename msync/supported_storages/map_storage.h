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

  bool push(const int64_t &time, const MsgType &msg) {
    if (!storage_.empty() && time <= storage_.rbegin()->first) {
      return false;
    }

    storage_.insert(std::make_pair(time, msg));

    // check if we should remove the old ones
    if (storage_.begin()->first < time - history_win_) {
      storage_.erase(storage_.begin());
    }

    return true;
  }

  size_t size() const { return storage_.size(); }

  bool empty() const { return storage_.empty(); }

  ConstIter end() const { return storage_.end(); }

  ConstIter find(const Time time) const { return storage_.find(time); }

  // nearest item with stamp NO LARGE(<=) than time
  ConstIter findPre(const Time time) const {
    auto upper = storage_.upper_bound(time);
    return upper != storage_.begin() ? std::prev(upper) : storage_.end();
  }

  // nearest item with stamp LARGE(>) than time
  ConstIter findSuc(const Time time) const {
    return storage_.upper_bound(time);
  }

  // the oldest item
  ConstIter front() const { return storage_.begin(); }

  // the newest item
  ConstIter back() const {
    if (empty())
      return storage_.end();
    return std::prev(storage_.end());
  }

protected:
  TimeMsgMap<_Msg, _Alloc> storage_;
};

} // namespace msync
