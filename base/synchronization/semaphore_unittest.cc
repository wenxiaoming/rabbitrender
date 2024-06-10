// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "synchronization/semaphore.h"

#include <thread>

#include "gtest/gtest.h"

TEST(SemaphoreTest, SimpleValidity) {
  fml::Semaphore sem(100);
  ASSERT_TRUE(sem.IsValid());
}

TEST(SemaphoreTest, WaitOnZero) {
  fml::Semaphore sem(0);
  ASSERT_FALSE(sem.TryWait());
}

TEST(SemaphoreTest, WaitOnZeroSignalThenWait) {
  fml::Semaphore sem(0);
  ASSERT_FALSE(sem.TryWait());
  std::thread thread([&sem]() { sem.Signal(); });
  thread.join();
  ASSERT_TRUE(sem.TryWait());
  ASSERT_FALSE(sem.TryWait());
}
