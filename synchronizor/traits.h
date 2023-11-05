#pragma once

namespace synchronizor  {

template <typename _T> struct PolicyTraits {};

template <typename _T> struct StorageTraits {};

template <typename _T> struct SyncronizerTraits {};

// default linear interpolater traits
template <typename T> struct LinearInterpolaterTraits {
  static T plus(const T &a, const T &b) { return a + b; }
  static T between(const T &from, const T &to) { return to - from; }
};

} // namespace sync
