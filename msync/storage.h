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

  // interfaces a storage must supports

  // append a message at tail
  bool push(const Time time, const MsgType &msg) {
    return derived().pushImpl(time, msg);
  }

  // how many message current in storage
  size_t size() const { return derived().sizeImpl(); }

  // if storage is empty
  bool empty() const { return derived().emptyImpl(); }

  // get the begin const iterator of storage
  ConstIter begin() const { return derived().beginImpl(); }

  // get the end const iterator of storage
  ConstIter end() const { return derived().endImpl(); }

  // find the item at given time
  ConstIter find(const Time time) const { return derived().findImpl(time); }

  // find nearest item with stamp NO LARGE(<=) than time
  ConstIter findPre(const Time time) const {
    return derived().findPreImpl(time);
  }

  // nearest item with stamp LARGE(>) than time
  ConstIter findSuc(const Time time) const {
    return derived().findSucImpl(time);
  }

  // the front (oldest, with smallest stamp) item
  // caller should make sure storage is not empty
  std::pair<Time, MsgType> front() const { return derived().frontImpl(); }

  // the back (newest, with largest stamp) item
  // caller should make sure storage is not empty
  std::pair<Time, MsgType> back() const { return derived().backImpl(); }

protected:
  Derived &derived() { return static_cast<Derived &>(*this); }
  const Derived &derived() const { return static_cast<const Derived &>(*this); }

  Time history_win_;
};

} // namespace msync
