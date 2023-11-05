#pragma once

#include <map>

#include "traits.h"
#include "types.h"

namespace msync {

template <typename _Derived> struct StorageBase {
  using Derived = _Derived;
  using MsgType = typename StorageTraits<Derived>::MsgType;
  using ConstIter = typename StorageTraits<Derived>::ConstIter;

  StorageBase(const Time history_win) : history_win_(history_win) {}

  // push at back
  bool push(const Time time, const MsgType &msg) {
    return derived().push(time, msg);
  }

  size_t size() const { return derived().size(); }

  bool empty() const { return derived().empty(); }

  ConstIter end() const { return derived().end(); }

protected:
  Derived &derived() { return static_cast<Derived>(*this); }
  const Derived &derived() const { return static_cast<Derived>(*this); }

  Time history_win_;
};

} // namespace msync
