#pragma once

#include "../policy.h"
#include "../traits.h"

namespace msync {

// Newest policy
template <typename _MsgType, typename _Storage> struct NewestPolicy;

template <typename _MsgType, typename _Storage>
struct PolicyTraits<NewestPolicy<_MsgType, _Storage>> {
  using MsgType = _MsgType;
  using InType = MsgType;
  using OutType = std::pair<MsgType, bool>;
  using Storage = _Storage;
};

template <typename _MsgType, typename _Storage>
struct NewestPolicy : public Policy<NewestPolicy<_MsgType, _Storage>> {
  using Base = Policy<NewestPolicy<_MsgType, _Storage>>;
  using MsgType = _MsgType;

  using Base::storage_;

  NewestPolicy(const Time history_win = 0, const PolicyAttribute attr = kNormal)
      : Base(history_win, attr) {}

  virtual std::pair<MsgType, bool> doPeek(const Time) const override {
    std::pair<MsgType, bool> ret;
    auto found = storage_.back();
    if (found == storage_.end()) {
      ret.second = false;
    } else {
      ret.first = found->second;
      ret.second = true;
    }
    return ret;
  }
};

} // namespace msync
