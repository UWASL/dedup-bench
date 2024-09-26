// Copyright 2017 The CRC32C Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef CRC32C_CRC32C_INTERNAL_H_
#define CRC32C_CRC32C_INTERNAL_H_

// Internal functions that may change between releases.

#include <cstddef>
#include <cstdint>

namespace crc32c {

// Un-accelerated implementation that works on all CPUs.
uint32_t ExtendPortable(uint32_t crc, const uint8_t* data, size_t count);

// CRCs are pre- and post- conditioned by xoring with all ones.
static constexpr const uint32_t kCRC32Xor = static_cast<uint32_t>(0xffffffffU);

inline uint32_t Crc32c(const uint8_t* data, size_t count) {
  return ExtendPortable(0, data, count);
}

inline uint32_t Crc32c(const char* data, size_t count) {
  return ExtendPortable(0, reinterpret_cast<const uint8_t*>(data), count);
}

// }
}  // namespace crc32c

#endif  // CRC32C_CRC32C_INTERNAL_H_
