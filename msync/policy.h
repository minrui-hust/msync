#pragma once

#include <vector>

#include "storage.h"
#include "traits.h"

namespace msync {

template <typename _Derived> struct PolicyBase {
  using Derived = _Derived;
  using InType = typename PolicyTraits<Derived>::InType;
  using OutType = typename PolicyTraits<Derived>::OutType;

  PolicyBase() {}

  virtual bool push(const Time time, const InType &msg) = 0;

  // get min successor of time
  virtual Time sucTime(const Time time, const PolicyAttribute attr) const = 0;

  // peek data given time
  virtual std::pair<OutType, StatusCode> peek(const Time time) const = 0;

protected:
  Derived &derived() { return static_cast<Derived>(*this); }
  const Derived &derived() const { return static_cast<Derived>(*this); }
};

template <typename _Derived> struct Policy;

template <typename _Derived> struct PolicyTraits<Policy<_Derived>> {
  using MsgType = typename PolicyTraits<_Derived>::MsgType;
  using InType = typename PolicyTraits<_Derived>::InType;
  using OutType = typename PolicyTraits<_Derived>::OutType;
  using Storage = typename PolicyTraits<_Derived>::Storage;
};

template <typename _Derived>
struct Policy : public PolicyBase<Policy<_Derived>> {
  using Base = PolicyBase<Policy<_Derived>>;
  using Derived = _Derived;
  using InType = typename PolicyTraits<Derived>::InType;
  using OutType = typename PolicyTraits<Derived>::OutType;
  using Storage = typename PolicyTraits<Derived>::Storage;

  Policy(const Time history_win, const PolicyAttribute attr = kNormal)
      : attr_(attr), storage_(history_win) {}

  virtual bool push(const Time time, const InType &msg) override {
    return storage_.push(time, msg);
  }

  virtual Time sucTime(const Time time,
                       const PolicyAttribute attr) const override {
    auto iter = storage_.findSuc(time);

    if (attr_ < attr || iter == storage_.end()) {
      return std::numeric_limits<Time>::max();
    } else {
      return iter->first;
    }
  }

  virtual std::pair<OutType, StatusCode> peek(const Time time) const override {
    auto out = doPeek(time);
    if (out.second) {
      return {out, kPeekSuccess};
    } else if (!storage_.empty() && time < storage_.back()->first) {
      return {out, kPeekExpired};
    } else {
      return {out, kPeekNotReady};
    }
  }

  virtual OutType doPeek(const Time time) const = 0;

  PolicyAttribute attr() const { return attr_; }

  size_t queueSize() const { return storage_.size(); }

protected:
  PolicyAttribute attr_;
  Storage storage_;
};

template <typename _Policy> struct PolicyArray;

template <typename _Policy> struct PolicyTraits<PolicyArray<_Policy>> {
  using MsgType = typename PolicyTraits<_Policy>::MsgType;
  using InType = std::pair<MsgType, int>;
  using OutType = std::vector<std::pair<MsgType, bool>>;
};

template <typename _Policy>
struct PolicyArray : public PolicyBase<PolicyArray<_Policy>> {
  using Policy = _Policy;
  using InType = typename PolicyTraits<PolicyArray>::InType;
  using OutType = typename PolicyTraits<PolicyArray>::OutType;

  PolicyArray(const std::vector<Policy> &policies) : policies_(policies) {}

  virtual bool push(const Time time, const InType &msg) override {
    return policies_.at(msg.second).push(time, msg.first);
  }

  virtual Time sucTime(const Time time,
                       const PolicyAttribute attr) const override {
    Time suc = std::numeric_limits<Time>::max();
    for (const auto &policy : policies_) {
      suc = std::min(suc, policy.sucTime(time, attr));
    }
    return suc;
  }

  virtual std::pair<OutType, StatusCode> peek(const Time time) const override {
    OutType out;
    StatusCode status;
    bool all_success = true;
    bool any_expire = false;

    for (const auto &policy : policies_) {
      auto res = policy.peek(time);
      out.emplace_back(res.first);
      if (policy.attr() != kOptional) {
        all_success &= (kPeekSuccess == res.second);
        any_expire |= (kPeekExpired == res.second);
      }
    }

    if (all_success) {
      status = kPeekSuccess;
    } else if (any_expire) {
      status = kPeekExpired;
    } else {
      status = kPeekNotReady;
    }

    return {out, status};
  }

  size_t queueSize(int id) const { return policies_.at(id).queueSize(); }

protected:
  std::vector<Policy> policies_;
};

} // namespace msync
