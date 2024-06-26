// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "synchronization/count_down_latch.h"

#include <chrono>
#include <thread>

#include "build_config.h"
#include "thread.h"
#include "gtest/gtest.h"

namespace fml {

TEST(CountDownLatchTest, CanWaitOnZero) {
  CountDownLatch latch(0);
  latch.Wait();
}

#if OS_FUCHSIA
#define CanWait DISABLED_CanWait
#else
#define CanWait CanWait
#endif
TEST(CountDownLatchTest, CanWait) {
  fml::Thread thread("test_thread");
  const size_t count = 100;
  size_t current_count = 0;
  CountDownLatch latch(count);
  auto decrement_latch_on_thread = [runner = thread.GetTaskRunner(), &latch,
                                    &current_count]() {
    runner->PostTask([&latch, &current_count]() {
      std::this_thread::sleep_for(std::chrono::microseconds(100));
      current_count++;
      latch.CountDown();
    });
  };
  for (size_t i = 0; i < count; ++i) {
    decrement_latch_on_thread();
  }
  latch.Wait();
  ASSERT_EQ(current_count, count);
}

}  // namespace fml
