#pragma once
#include <functional>
#include <iostream>
#include <limits>
#include <memory>
#include <queue>
#include <tuple>
#include <utility>

#include "policy.h"

namespace msync {

template <typename _Derived> struct SyncronizerBase {
  using Derived = _Derived;

  using PolicyTuple = typename SyncronizerTraits<Derived>::PolicyTuple;

  using CallbackFunction =
      typename SyncronizerTraits<Derived>::CallbackFunction;

  static constexpr size_t kNumPolicies = std::tuple_size<PolicyTuple>::value;

  template <size_t _Idx> using Policy = std::tuple_element_t<_Idx, PolicyTuple>;

  template <size_t _Idx>
  using PolicyInType = typename PolicyTraits<Policy<_Idx>>::InType;

  template <size_t _Idx>
  using PolicyOutType = typename PolicyTraits<Policy<_Idx>>::OutType;

  template <typename... _Polices>
  SyncronizerBase(const PolicyAttribute attr, const _Polices &...policies)
      : interest_attr_(attr), policies_(policies...) {
    time_pivot_ = -1;
  }

  template <size_t _Idx = 0>
  StatusCode push(const int64_t time, const PolicyInType<_Idx> &msg) {
    if (std::get<_Idx>(policies_).push(time, msg)) {
      return checkQueue();
    } else {
      return kMsgDropped;
    }
  }

  void registerCallback(const CallbackFunction &cb) { cb_ = cb; }

  Time timePivot() const { return time_pivot_; }

  template <size_t _Idx = 0> size_t queueSize() const {
    return std::get<_Idx>(policies_).queueSize();
  }

  template <size_t _Idx = 0> size_t queueSize(int id) const {
    return std::get<_Idx>(policies_).queueSize(id);
  }

protected:
  virtual void updatePivot(const Time time, const StatusCode code) = 0;

  StatusCode checkQueue() {
    Time time;
    StatusCode emit_status;
    bool any_emitted = false;

    do {
      time = sucTime(time_pivot_, interest_attr_);
      if (time == std::numeric_limits<Time>::max())
        break;

      emit_status = tryEmit(time);
      updatePivot(time, emit_status);

      any_emitted |= (kEmitSuccess == emit_status);
    } while (emit_status > kEmitNotReady);

    return any_emitted ? kMsgEmitted : kMsgAccepted;
  }

  Time sucTime(const Time time, const PolicyAttribute attr) const {
    return sucTimeHelper<kNumPolicies, true>(time, attr);
  }

  // if _Idx != 0, sucTimeHelper match this function
  template <size_t _Idx, typename std::enable_if_t<_Idx != 0, bool>>
  Time sucTimeHelper(const Time time, const PolicyAttribute attr) const {
    const auto &policy = std::get<_Idx - 1>(policies_);

    return std::min(policy.sucTime(time, attr),
                    sucTimeHelper<_Idx - 1, true>(time, attr));
  }

  // if _Idx == 0, findNearestSuc match this function
  template <size_t _Idx, typename std::enable_if_t<_Idx == 0, bool>>
  Time sucTimeHelper(const Time, const PolicyAttribute) const {
    return std::numeric_limits<Time>::max();
  }

  StatusCode tryEmit(const Time time) {
    return emitHelper<kNumPolicies, true>(time);
  }

  // match when _Idx > 0
  template <size_t _Idx, typename std::enable_if_t<_Idx != 0, bool> _,
            typename... _Msgs>
  StatusCode emitHelper(const int64_t time, const _Msgs &...msgs) {
    const auto &policy = std::get<_Idx - 1>(policies_);
    auto res = policy.peek(time);

    if (kPeekSuccess == res.second) {
      return emitHelper<_Idx - 1, true>(time, res.first, msgs...);
    } else if (kPeekExpired == res.second) {
      return kEmitExpired;
    } else {
      return kEmitNotReady;
    }
  }

  // match when _Idx == 0
  template <size_t _Idx, typename std::enable_if_t<_Idx == 0, bool> _,
            typename... _Msgs>
  StatusCode emitHelper(const int64_t time, const _Msgs &...msgs) {
    if (cb_)
      cb_(time, msgs...);
    return kEmitSuccess;
  }

  Time time_pivot_;
  PolicyAttribute interest_attr_;
  PolicyTuple policies_;
  CallbackFunction cb_;
};

template <typename... _Polices> struct SyncronizerMinInterval;

template <typename... _Polices>
struct SyncronizerTraits<SyncronizerMinInterval<_Polices...>> {
  using PolicyTuple = std::tuple<_Polices...>;

  using CallbackFunction = std::function<void(
      const int64_t time,
      const typename PolicyTraits<_Polices>::OutType &...msgs)>;
};

template <typename... _Polices>
struct SyncronizerMinInterval
    : public SyncronizerBase<SyncronizerMinInterval<_Polices...>> {
  using Base = SyncronizerBase<SyncronizerMinInterval<_Polices...>>;

  using Base::kNumPolicies;
  using Base::policies_;
  using Base::time_pivot_;

  // constructor
  SyncronizerMinInterval(const int64_t min_interval,
                         const _Polices &...policies)
      : Base(kNormal, policies...) {
    min_interval_ = min_interval;
  }

protected:
  virtual void updatePivot(const Time time, const StatusCode code) override {
    if (code == kEmitSuccess) {
      time_pivot_ = time + min_interval_ - 1;
    } else if (code == kEmitExpired) {
      time_pivot_ = time;
    }
  }

  Time min_interval_;
};

template <typename... _Polices> struct SyncronizerMasterSlave;

template <typename... _Polices>
struct SyncronizerTraits<SyncronizerMasterSlave<_Polices...>> {
  using PolicyTuple = std::tuple<_Polices...>;

  using CallbackFunction = std::function<void(
      const int64_t time,
      const typename PolicyTraits<_Polices>::OutType &...msgs)>;
};

template <typename... _Polices>
struct SyncronizerMasterSlave
    : public SyncronizerBase<SyncronizerMasterSlave<_Polices...>> {
  using Base = SyncronizerBase<SyncronizerMasterSlave<_Polices...>>;

  using Base::kNumPolicies;
  using Base::policies_;
  using Base::time_pivot_;

  // constructor
  SyncronizerMasterSlave(const _Polices &...policies)
      : Base(kMaster, policies...) {}

protected:
  virtual void updatePivot(const Time time, const StatusCode code) override {
    if (code > kEmitNotReady) {
      time_pivot_ = time;
    }
  }
};

template <typename _Policy>
using HomoSyncronizerMinInterval = SyncronizerMinInterval<PolicyArray<_Policy>>;

template <typename _Policy>
using HomoSyncronizerMasterSlave = SyncronizerMasterSlave<PolicyArray<_Policy>>;

} // namespace msync
