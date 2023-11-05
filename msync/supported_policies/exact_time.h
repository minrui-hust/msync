#pragma once

#include "../policy.h"
#include "../traits.h"

namespace msync {

// Exact time policy
template <typename _MsgType, typename _Storage> struct ExactTimePolicy;

template <typename _MsgType, typename _Storage>
struct PolicyTraits<ExactTimePolicy<_MsgType, _Storage>> {
  using MsgType = _MsgType;
  using InType = MsgType;
  using OutType = std::pair<MsgType, bool>;
  using Storage = _Storage;
};

template <typename _MsgType, typename _Storage>
struct ExactTimePolicy : public Policy<ExactTimePolicy<_MsgType, _Storage>> {
  using Base = Policy<ExactTimePolicy<_MsgType, _Storage>>;
  using MsgType = _MsgType;

  using Base::storage_;

  ExactTimePolicy(const Time history_win = 1e6,
                  const PolicyAttribute attr = kNormal)
      : Base(history_win, attr) {}

  virtual std::pair<MsgType, bool> doPeek(const Time time) const override {
    std::pair<MsgType, bool> ret;
    auto found = storage_.find(time);
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
