#pragma once

#include "../policy.h"
#include "../traits.h"

namespace msync {

// Nearest policy
template <typename _MsgType, typename _Storage> struct NearestPolicy;

template <typename _MsgType, typename _Storage>
struct PolicyTraits<NearestPolicy<_MsgType, _Storage>> {
  using MsgType = _MsgType;
  using InType = MsgType;
  using OutType = std::pair<MsgType, bool>;
  using Storage = _Storage;
};

template <typename _MsgType, typename _Storage>
struct NearestPolicy : public Policy<NearestPolicy<_MsgType, _Storage>> {
  using Base = Policy<NearestPolicy<_MsgType, _Storage>>;
  using MsgType = _MsgType;

  using Base::storage_;

  NearestPolicy(const Time history_win = 1e6, const Time valid_win = 5e5,
                const PolicyAttribute attr = kNormal)
      : Base(history_win, attr), valid_win_(valid_win) {}

  virtual std::pair<MsgType, bool> doPeek(const Time time) const override {
    std::pair<MsgType, bool> ret;
    ret.second = false;

    Time delta_min = std::numeric_limits<Time>::max();
    int sel = -1;
    auto pre = storage_.findPre(time);
    auto suc = storage_.findSuc(time);

    if (pre != storage_.end()) {
      Time delta = std::abs(pre->first - time);
      if (delta < delta_min) {
        delta_min = delta;
        sel = 0;
      }
    }

    if (suc != storage_.end()) {
      Time delta = std::abs(suc->first - time);
      if (delta < delta_min) {
        delta_min = delta;
        sel = 1;
      }
    }

    if (delta_min > valid_win_ || sel < 0)
      return ret;

    ret.first = sel == 0 ? pre->second : suc->second;
    ret.second = true;
    return ret;
  }

protected:
  Time valid_win_;
};

} // namespace msync
