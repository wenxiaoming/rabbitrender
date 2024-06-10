// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "trace_event.h"

#include <algorithm>
#include <atomic>
#include <utility>

#include "ascii_trie.h"
#include "build_config.h"
#include "logging.h"

namespace fml {
namespace tracing {

#if FLUTTER_TIMELINE_ENABLED

namespace {
AsciiTrie gAllowlist;
TimelineEventHandler gTimelineEventHandler;

inline void FlutterTimelineEvent(const char* label,
                                 int64_t timestamp0,
                                 int64_t timestamp1_or_async_id,
                                 Dart_Timeline_Event_Type type,
                                 intptr_t argument_count,
                                 const char** argument_names,
                                 const char** argument_values) {
  if (gTimelineEventHandler && gAllowlist.Query(label)) {
    gTimelineEventHandler(label, timestamp0, timestamp1_or_async_id, type,
                          argument_count, argument_names, argument_values);
  }
}
}  // namespace

void TraceSetAllowlist(const std::vector<std::string>& allowlist) {
  gAllowlist.Fill(allowlist);
}

void TraceSetTimelineEventHandler(TimelineEventHandler handler) {
  gTimelineEventHandler = handler;
}

size_t TraceNonce() {
  static std::atomic_size_t gLastItem;
  return ++gLastItem;
}

void TraceTimelineEvent(TraceArg category_group,
                        TraceArg name,
                        int64_t timestamp_micros,
                        TraceIDArg identifier,
                        Dart_Timeline_Event_Type type,
                        const std::vector<const char*>& c_names,
                        const std::vector<std::string>& values) {
  const auto argument_count = std::min(c_names.size(), values.size());

  std::vector<const char*> c_values;
  c_values.resize(argument_count, nullptr);

  for (size_t i = 0; i < argument_count; i++) {
    c_values[i] = values[i].c_str();
  }

  FlutterTimelineEvent(
      name,                                      // label
      timestamp_micros,                          // timestamp0
      identifier,                                // timestamp1_or_async_id
      type,                                      // event type
      argument_count,                            // argument_count
      const_cast<const char**>(c_names.data()),  // argument_names
      c_values.data()                            // argument_values
  );
}

void TraceTimelineEvent(TraceArg category_group,
                        TraceArg name,
                        TraceIDArg identifier,
                        Dart_Timeline_Event_Type type,
                        const std::vector<const char*>& c_names,
                        const std::vector<std::string>& values) {
  TraceTimelineEvent(category_group,            // group
                     name,                      // name
                     Dart_TimelineGetMicros(),  // timestamp_micros
                     identifier,                // identifier
                     type,                      // type
                     c_names,                   // names
                     values                     // values
  );
}

void TraceEvent0(TraceArg category_group, TraceArg name) {
  FlutterTimelineEvent(name,                       // label
                       Dart_TimelineGetMicros(),   // timestamp0
                       0,                          // timestamp1_or_async_id
                       Dart_Timeline_Event_Begin,  // event type
                       0,                          // argument_count
                       nullptr,                    // argument_names
                       nullptr                     // argument_values
  );
}

void TraceEvent1(TraceArg category_group,
                 TraceArg name,
                 TraceArg arg1_name,
                 TraceArg arg1_val) {
  const char* arg_names[] = {arg1_name};
  const char* arg_values[] = {arg1_val};
  FlutterTimelineEvent(name,                       // label
                       Dart_TimelineGetMicros(),   // timestamp0
                       0,                          // timestamp1_or_async_id
                       Dart_Timeline_Event_Begin,  // event type
                       1,                          // argument_count
                       arg_names,                  // argument_names
                       arg_values                  // argument_values
  );
}

void TraceEvent2(TraceArg category_group,
                 TraceArg name,
                 TraceArg arg1_name,
                 TraceArg arg1_val,
                 TraceArg arg2_name,
                 TraceArg arg2_val) {
  const char* arg_names[] = {arg1_name, arg2_name};
  const char* arg_values[] = {arg1_val, arg2_val};
  FlutterTimelineEvent(name,                       // label
                       Dart_TimelineGetMicros(),   // timestamp0
                       0,                          // timestamp1_or_async_id
                       Dart_Timeline_Event_Begin,  // event type
                       2,                          // argument_count
                       arg_names,                  // argument_names
                       arg_values                  // argument_values
  );
}

void TraceEventEnd(TraceArg name) {
  FlutterTimelineEvent(name,                      // label
                       Dart_TimelineGetMicros(),  // timestamp0
                       0,                         // timestamp1_or_async_id
                       Dart_Timeline_Event_End,   // event type
                       0,                         // argument_count
                       nullptr,                   // argument_names
                       nullptr                    // argument_values
  );
}

void TraceEventAsyncBegin0(TraceArg category_group,
                           TraceArg name,
                           TraceIDArg id) {
  FlutterTimelineEvent(name,                      // label
                       Dart_TimelineGetMicros(),  // timestamp0
                       id,                        // timestamp1_or_async_id
                       Dart_Timeline_Event_Async_Begin,  // event type
                       0,                                // argument_count
                       nullptr,                          // argument_names
                       nullptr                           // argument_values
  );
}

void TraceEventAsyncEnd0(TraceArg category_group,
                         TraceArg name,
                         TraceIDArg id) {
  FlutterTimelineEvent(name,                           // label
                       Dart_TimelineGetMicros(),       // timestamp0
                       id,                             // timestamp1_or_async_id
                       Dart_Timeline_Event_Async_End,  // event type
                       0,                              // argument_count
                       nullptr,                        // argument_names
                       nullptr                         // argument_values
  );
}

void TraceEventAsyncBegin1(TraceArg category_group,
                           TraceArg name,
                           TraceIDArg id,
                           TraceArg arg1_name,
                           TraceArg arg1_val) {
  const char* arg_names[] = {arg1_name};
  const char* arg_values[] = {arg1_val};
  FlutterTimelineEvent(name,                      // label
                       Dart_TimelineGetMicros(),  // timestamp0
                       id,                        // timestamp1_or_async_id
                       Dart_Timeline_Event_Async_Begin,  // event type
                       1,                                // argument_count
                       arg_names,                        // argument_names
                       arg_values                        // argument_values
  );
}

void TraceEventAsyncEnd1(TraceArg category_group,
                         TraceArg name,
                         TraceIDArg id,
                         TraceArg arg1_name,
                         TraceArg arg1_val) {
  const char* arg_names[] = {arg1_name};
  const char* arg_values[] = {arg1_val};
  FlutterTimelineEvent(name,                           // label
                       Dart_TimelineGetMicros(),       // timestamp0
                       id,                             // timestamp1_or_async_id
                       Dart_Timeline_Event_Async_End,  // event type
                       1,                              // argument_count
                       arg_names,                      // argument_names
                       arg_values                      // argument_values
  );
}

void TraceEventInstant0(TraceArg category_group, TraceArg name) {
  FlutterTimelineEvent(name,                         // label
                       Dart_TimelineGetMicros(),     // timestamp0
                       0,                            // timestamp1_or_async_id
                       Dart_Timeline_Event_Instant,  // event type
                       0,                            // argument_count
                       nullptr,                      // argument_names
                       nullptr                       // argument_values
  );
}

void TraceEventInstant1(TraceArg category_group,
                        TraceArg name,
                        TraceArg arg1_name,
                        TraceArg arg1_val) {
  const char* arg_names[] = {arg1_name};
  const char* arg_values[] = {arg1_val};
  FlutterTimelineEvent(name,                         // label
                       Dart_TimelineGetMicros(),     // timestamp0
                       0,                            // timestamp1_or_async_id
                       Dart_Timeline_Event_Instant,  // event type
                       1,                            // argument_count
                       arg_names,                    // argument_names
                       arg_values                    // argument_values
  );
}

void TraceEventInstant2(TraceArg category_group,
                        TraceArg name,
                        TraceArg arg1_name,
                        TraceArg arg1_val,
                        TraceArg arg2_name,
                        TraceArg arg2_val) {
  const char* arg_names[] = {arg1_name, arg2_name};
  const char* arg_values[] = {arg1_val, arg2_val};
  FlutterTimelineEvent(name,                         // label
                       Dart_TimelineGetMicros(),     // timestamp0
                       0,                            // timestamp1_or_async_id
                       Dart_Timeline_Event_Instant,  // event type
                       2,                            // argument_count
                       arg_names,                    // argument_names
                       arg_values                    // argument_values
  );
}

void TraceEventFlowBegin0(TraceArg category_group,
                          TraceArg name,
                          TraceIDArg id) {
  FlutterTimelineEvent(name,                      // label
                       Dart_TimelineGetMicros(),  // timestamp0
                       id,                        // timestamp1_or_async_id
                       Dart_Timeline_Event_Flow_Begin,  // event type
                       0,                               // argument_count
                       nullptr,                         // argument_names
                       nullptr                          // argument_values
  );
}

void TraceEventFlowStep0(TraceArg category_group,
                         TraceArg name,
                         TraceIDArg id) {
  FlutterTimelineEvent(name,                           // label
                       Dart_TimelineGetMicros(),       // timestamp0
                       id,                             // timestamp1_or_async_id
                       Dart_Timeline_Event_Flow_Step,  // event type
                       0,                              // argument_count
                       nullptr,                        // argument_names
                       nullptr                         // argument_values
  );
}

void TraceEventFlowEnd0(TraceArg category_group, TraceArg name, TraceIDArg id) {
  FlutterTimelineEvent(name,                          // label
                       Dart_TimelineGetMicros(),      // timestamp0
                       id,                            // timestamp1_or_async_id
                       Dart_Timeline_Event_Flow_End,  // event type
                       0,                             // argument_count
                       nullptr,                       // argument_names
                       nullptr                        // argument_values
  );
}

#else  // FLUTTER_TIMELINE_ENABLED

void TraceSetAllowlist(const std::vector<std::string>& allowlist) {}

void TraceSetTimelineEventHandler(TimelineEventHandler handler) {}

size_t TraceNonce() {
  return 0;
}

void TraceTimelineEvent(TraceArg category_group,
                        TraceArg name,
                        int64_t timestamp_micros,
                        TraceIDArg identifier,
                        Dart_Timeline_Event_Type type,
                        const std::vector<const char*>& c_names,
                        const std::vector<std::string>& values) {}

void TraceTimelineEvent(TraceArg category_group,
                        TraceArg name,
                        TraceIDArg identifier,
                        Dart_Timeline_Event_Type type,
                        const std::vector<const char*>& c_names,
                        const std::vector<std::string>& values) {}

void TraceEvent0(TraceArg category_group, TraceArg name) {}

void TraceEvent1(TraceArg category_group,
                 TraceArg name,
                 TraceArg arg1_name,
                 TraceArg arg1_val) {}

void TraceEvent2(TraceArg category_group,
                 TraceArg name,
                 TraceArg arg1_name,
                 TraceArg arg1_val,
                 TraceArg arg2_name,
                 TraceArg arg2_val) {}

void TraceEventEnd(TraceArg name) {}

void TraceEventAsyncComplete(TraceArg category_group,
                             TraceArg name,
                             TimePoint begin,
                             TimePoint end) {}

void TraceEventAsyncBegin0(TraceArg category_group,
                           TraceArg name,
                           TraceIDArg id) {}

void TraceEventAsyncEnd0(TraceArg category_group,
                         TraceArg name,
                         TraceIDArg id) {}

void TraceEventAsyncBegin1(TraceArg category_group,
                           TraceArg name,
                           TraceIDArg id,
                           TraceArg arg1_name,
                           TraceArg arg1_val) {}

void TraceEventAsyncEnd1(TraceArg category_group,
                         TraceArg name,
                         TraceIDArg id,
                         TraceArg arg1_name,
                         TraceArg arg1_val) {}

void TraceEventInstant0(TraceArg category_group, TraceArg name) {}

void TraceEventInstant1(TraceArg category_group,
                        TraceArg name,
                        TraceArg arg1_name,
                        TraceArg arg1_val) {}

void TraceEventInstant2(TraceArg category_group,
                        TraceArg name,
                        TraceArg arg1_name,
                        TraceArg arg1_val,
                        TraceArg arg2_name,
                        TraceArg arg2_val) {}

void TraceEventFlowBegin0(TraceArg category_group,
                          TraceArg name,
                          TraceIDArg id) {}

void TraceEventFlowStep0(TraceArg category_group,
                         TraceArg name,
                         TraceIDArg id) {}

void TraceEventFlowEnd0(TraceArg category_group, TraceArg name, TraceIDArg id) {
}

#endif  // FLUTTER_TIMELINE_ENABLED

}  // namespace tracing
}  // namespace fml
