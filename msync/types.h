#pragma once
#include <cstdint>

namespace msync {

using Time = int64_t;

enum PolicyAttribute {
  kOptional = 0,
  kNormal,
  kMaster,
};

enum StatusCode {
  kMsgDropped = 0,
  kMsgAccepted,
  kMsgEmitted,

  kPeekNotReady = 10,
  kPeekExpired,
  kPeekSuccess,

  kEmitNotReady = 20,
  kEmitExpired,
  kEmitSuccess,
};

} // namespace msync
