#pragma once

#include "../policy.h"
#include "../traits.h"

namespace msync {

// Linear interpolate policy
template <typename _MsgType, typename _Storage> struct LinearInterpolatePolicy;

template <typename _MsgType, typename _Storage>
struct PolicyTraits<LinearInterpolatePolicy<_MsgType, _Storage>> {
  using MsgType = _MsgType;
  using InType = MsgType;
  using OutType = std::pair<MsgType, bool>;
  using Storage = _Storage;
};

template <typename _MsgType, typename _Storage>
struct LinearInterpolatePolicy
    : public Policy<LinearInterpolatePolicy<_MsgType, _Storage>> {
  using Base = Policy<LinearInterpolatePolicy<_MsgType, _Storage>>;
  using MsgType = _MsgType;

  using Base::storage_;

  LinearInterpolatePolicy(const Time history_win = 1e6,
                          const Time predict_win = 5e5,
                          const PolicyAttribute attr = kNormal)
      : Base(history_win, attr), predict_win_(predict_win) {}

  virtual std::pair<MsgType, bool> doPeek(const Time time) const override {
    std::pair<MsgType, bool> ret;
    ret.second = false;

    auto pre = storage_.findPre(time);
    auto suc = storage_.findSuc(time);

    if (pre == storage_.end())
      return ret;

    auto low = pre;
    auto hig = suc;
    if (suc == storage_.end() && storage_.size() < 2 && time != pre->first)
      return ret;

    if (time == pre->first) {
      ret.first = pre->second;
      ret.second = true;
      return ret;
    }

    if (suc == storage_.end()) {
      low = std::prev(pre);
      hig = pre;
    }

    // extapolate too far
    if (time - hig->first > predict_win_) {
      return ret;
    }

    // do interpolate
    double ratio = (time - low->first) / double(hig->first - low->first);
    ret.first = LinearInterpolaterTraits<MsgType>::plus(
        low->second,
        LinearInterpolaterTraits<MsgType>::between(low->second, hig->second) *
            ratio);
    ret.second = true;
    return ret;
  }

protected:
  Time predict_win_;
};

} // namespace msync
