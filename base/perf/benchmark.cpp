/*
 * Copyright (C) 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "performancecounters.h"

#include "gtest/gtest.h"
#include "benchmark.h"
#include "profiler.h"


#include <vector>
#include <random>

using namespace utils;

int64_t gcd(int64_t x, int64_t y) {
    if (y)
        gcd(y, x % y);
    else
        return x;
}

#define SYSTRACE_VALUE32 printf
TEST(PerfTest, Test) {
    Profiler profiler;

    // we want to remove all this when tracing is completely disabled
    profiler.resetEvents(Profiler::EV_CPU_CYCLES  | Profiler::EV_BPU_MISSES);
    profiler.start();

    // execute
    gcd(10251000, 417500);

    profiler.stop();
    UTILS_UNUSED Profiler::Counters counters = profiler.readCounters();
    printf("GLThread (I):%lld\n", counters.getInstructions());
    printf("GLThread (C):%lld\n", counters.getCpuCycles());
    printf("GLThread (CPI x10):%f\n", counters.getCPI() * 10);
    printf("GLThread (BPU miss):%lld\n", counters.getBranchMisses());

    if (counters.getBranchMisses() != 0)
    printf("GLThread (I / BPU miss):%f\n",
            counters.getInstructions() / counters.getBranchMisses());

}