#pragma once
#include <cstdint>

namespace msync {

using Time = int64_t;

enum PolicyAttribute {
  kOptional = 0, // Optional policy, which means peek fail is ok
  kNormal,       // Normal policy, peek fail will not do callback
  kMaster,       // Master policy
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
