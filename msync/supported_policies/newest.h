#pragma once

#include "../policy.h"
#include "../traits.h"

#include "../supported_storages/map_storage.h"

namespace msync {

// Newest policy, default to use MapStorage
template <typename _MsgType,
          typename _Alloc = std::allocator<std::pair<const Time, _MsgType>>,
          typename _Storage = MapStorage<_MsgType, _Alloc>>
struct NewestPolicy;

template <typename _MsgType, typename _Alloc, typename _Storage>
struct PolicyTraits<NewestPolicy<_MsgType, _Alloc, _Storage>> {
  using MsgType = _MsgType;
  using InType = MsgType;
  using OutType = std::pair<MsgType, bool>;
  using Storage = _Storage;
};

template <typename _MsgType, typename _Alloc, typename _Storage>
struct NewestPolicy : public Policy<NewestPolicy<_MsgType, _Alloc, _Storage>> {
  using Base = Policy<NewestPolicy<_MsgType, _Alloc, _Storage>>;
  using MsgType = _MsgType;

  using Base::storage_;

  NewestPolicy(const Time history_win = 0, const PolicyAttribute attr = kNormal)
      : Base(history_win, attr) {}

  virtual std::pair<MsgType, bool> doPeek(const Time) const override {
    std::pair<MsgType, bool> ret{MsgType{}, false};

    if (!storage_.empty()) {
      ret.first = storage_.back().second;
      ret.second = true;
    }

    return ret;
  }
};

} // namespace msync
