/*
 *
 * Copyright (C) 2019 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "test_harness/test_harness.hpp"
#include "gtest/gtest.h"
#include "utils/utils.hpp"
#include <level_zero/ze_api.h>

namespace lzt = level_zero_tests;

namespace level_zero_tests {

ze_command_list_handle_t create_command_list() {
  return create_command_list(zeDevice::get_instance()->get_device());
}
ze_command_list_handle_t create_command_list(ze_device_handle_t device) {
  return create_command_list(device, 0);
}

ze_command_list_handle_t create_command_list(ze_device_handle_t device,
                                             ze_command_list_flags_t flags) {
  return create_command_list(lzt::get_default_context(), device, 0);
}

ze_command_list_handle_t create_command_list(ze_context_handle_t context,
                                             ze_device_handle_t device,
                                             ze_command_list_flags_t flags) {
  return create_command_list(context, device, flags, 0);
}

ze_command_list_handle_t create_command_list(ze_context_handle_t context,
                                             ze_device_handle_t device,
                                             ze_command_list_flags_t flags,
                                             uint32_t ordinal) {
  ze_command_list_desc_t descriptor = {};
  descriptor.stype = ZE_STRUCTURE_TYPE_COMMAND_LIST_DESC;

  descriptor.pNext = nullptr;
  descriptor.flags = flags;
  descriptor.commandQueueGroupOrdinal = ordinal;
  ze_command_list_handle_t command_list = nullptr;
  auto context_initial = context;
  auto device_initial = device;
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeCommandListCreate(context, device, &descriptor, &command_list));
  EXPECT_EQ(context, context_initial);
  EXPECT_EQ(device, device_initial);
  EXPECT_NE(nullptr, command_list);

  return command_list;
}

ze_command_list_handle_t create_immediate_command_list() {
  return create_immediate_command_list(zeDevice::get_instance()->get_device());
}

ze_command_list_handle_t
create_immediate_command_list(ze_device_handle_t device) {
  return create_immediate_command_list(device, 0, ZE_COMMAND_QUEUE_MODE_DEFAULT,
                                       ZE_COMMAND_QUEUE_PRIORITY_NORMAL, 0);
}

ze_command_list_handle_t create_immediate_command_list(
    ze_device_handle_t device, ze_command_queue_flags_t flags,
    ze_command_queue_mode_t mode, ze_command_queue_priority_t priority,
    uint32_t ordinal) {
  return create_immediate_command_list(lzt::get_default_context(), device,
                                       flags, mode, priority, ordinal);
}

ze_command_list_handle_t create_immediate_command_list(
    ze_context_handle_t context, ze_device_handle_t device,
    ze_command_queue_flags_t flags, ze_command_queue_mode_t mode,
    ze_command_queue_priority_t priority, uint32_t ordinal) {
  ze_command_queue_desc_t descriptor = {};
  descriptor.stype = ZE_STRUCTURE_TYPE_COMMAND_QUEUE_DESC;

  descriptor.pNext = nullptr;
  descriptor.flags = flags;
  descriptor.mode = mode;
  descriptor.priority = priority;
  descriptor.ordinal = ordinal;
  ze_command_list_handle_t command_list = nullptr;
  auto context_initial = context;
  auto device_initial = device;
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeCommandListCreateImmediate(context, device, &descriptor,
                                         &command_list));
  EXPECT_EQ(context, context_initial);
  EXPECT_EQ(device, device_initial);
  EXPECT_NE(nullptr, command_list);
  return command_list;
}

zeCommandList::zeCommandList() { command_list_ = create_command_list(); }

zeCommandList::~zeCommandList() {
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListDestroy(command_list_));
}

void append_memory_set(ze_command_list_handle_t cl, void *dstptr,
                       const uint8_t *value, size_t size) {
  append_memory_set(cl, dstptr, value, size, nullptr);
}

void append_memory_set(ze_command_list_handle_t cl, void *dstptr,
                       const uint8_t *value, size_t size,
                       ze_event_handle_t hSignalEvent) {
  append_memory_fill(cl, dstptr, value, 1, size, hSignalEvent);
}

void append_memory_fill(ze_command_list_handle_t cl, void *dstptr,
                        const void *pattern, size_t pattern_size, size_t size,
                        ze_event_handle_t hSignalEvent) {
  append_memory_fill(cl, dstptr, pattern, pattern_size, size, hSignalEvent, 0,
                     nullptr);
}

void append_memory_fill(ze_command_list_handle_t cl, void *dstptr,
                        const void *pattern, size_t pattern_size, size_t size,
                        ze_event_handle_t hSignalEvent,
                        uint32_t num_wait_events,
                        ze_event_handle_t *wait_events) {
  auto command_list_initial = cl;
  auto signal_event_initial = hSignalEvent;

  std::vector<ze_event_handle_t> wait_events_initial(num_wait_events);
  std::memcpy(wait_events_initial.data(), wait_events,
              sizeof(ze_event_handle_t) * num_wait_events);
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListAppendMemoryFill(
                                   cl, dstptr, pattern, pattern_size, size,
                                   hSignalEvent, num_wait_events, wait_events));
  EXPECT_EQ(cl, command_list_initial);
  EXPECT_EQ(hSignalEvent, signal_event_initial);
  for (int i = 0; i < num_wait_events; i++) {
    EXPECT_EQ(wait_events[i], wait_events_initial[i]);
  }
}

void append_memory_copy(ze_command_list_handle_t cl, void *dstptr,
                        const void *srcptr, size_t size,
                        ze_event_handle_t hSignalEvent) {
  append_memory_copy(cl, dstptr, srcptr, size, hSignalEvent, 0, nullptr);
}

void append_memory_copy(ze_command_list_handle_t cl, void *dstptr,
                        const void *srcptr, size_t size) {
  append_memory_copy(cl, dstptr, srcptr, size, nullptr);
}

void append_memory_copy(ze_command_list_handle_t cl, void *dstptr,
                        const void *srcptr, size_t size,
                        ze_event_handle_t hSignalEvent,
                        uint32_t num_wait_events,
                        ze_event_handle_t *wait_events) {

  auto command_list_initial = cl;
  auto signal_event_initial = hSignalEvent;
  std::vector<ze_event_handle_t> wait_events_initial(num_wait_events);
  std::memcpy(wait_events_initial.data(), wait_events,
              sizeof(ze_event_handle_t) * num_wait_events);
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListAppendMemoryCopy(
                                   cl, dstptr, srcptr, size, hSignalEvent,
                                   num_wait_events, wait_events));

  EXPECT_EQ(cl, command_list_initial);
  EXPECT_EQ(hSignalEvent, signal_event_initial);
  for (int i = 0; i < num_wait_events; i++) {
    EXPECT_EQ(wait_events[i], wait_events_initial[i]);
  }
}

void append_memory_copy(ze_context_handle_t src_context,
                        ze_command_list_handle_t cl, void *dstptr,
                        const void *srcptr, size_t size,
                        ze_event_handle_t hSignalEvent,
                        uint32_t num_wait_events,
                        ze_event_handle_t *wait_events) {

  auto command_list_initial = cl;
  auto signal_event_initial = hSignalEvent;
  std::vector<ze_event_handle_t> wait_events_initial(num_wait_events);
  std::memcpy(wait_events_initial.data(), wait_events,
              sizeof(ze_event_handle_t) * num_wait_events);
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListAppendMemoryCopyFromContext(
                                   cl, dstptr, src_context, srcptr, size,
                                   hSignalEvent, num_wait_events, wait_events));

  EXPECT_EQ(cl, command_list_initial);
  EXPECT_EQ(hSignalEvent, signal_event_initial);
  for (int i = 0; i < num_wait_events; i++) {
    EXPECT_EQ(wait_events[i], wait_events_initial[i]);
  }
}

void append_memory_copy_region(ze_command_list_handle_t hCommandList,
                               void *dstptr, const ze_copy_region_t *dstRegion,
                               uint32_t dstPitch, uint32_t dstSlicePitch,
                               const void *srcptr,
                               const ze_copy_region_t *srcRegion,
                               uint32_t srcPitch, uint32_t srcSlicePitch,
                               ze_event_handle_t hSignalEvent) {
  append_memory_copy_region(hCommandList, dstptr, dstRegion, dstPitch,
                            dstSlicePitch, srcptr, srcRegion, srcPitch,
                            srcSlicePitch, hSignalEvent, 0, nullptr);
}

void append_memory_copy_region(ze_command_list_handle_t hCommandList,
                               void *dstptr, const ze_copy_region_t *dstRegion,
                               uint32_t dstPitch, uint32_t dstSlicePitch,
                               const void *srcptr,
                               const ze_copy_region_t *srcRegion,
                               uint32_t srcPitch, uint32_t srcSlicePitch,
                               ze_event_handle_t hSignalEvent,
                               uint32_t num_wait_events,
                               ze_event_handle_t *wait_events) {
  auto command_list_initial = hCommandList;
  auto signal_event_initial = hSignalEvent;
  std::vector<ze_event_handle_t> wait_events_initial(num_wait_events);
  std::memcpy(wait_events_initial.data(), wait_events,
              sizeof(ze_event_handle_t) * num_wait_events);
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeCommandListAppendMemoryCopyRegion(
                hCommandList, dstptr, dstRegion, dstPitch, dstSlicePitch,
                srcptr, srcRegion, srcPitch, srcSlicePitch, hSignalEvent,
                num_wait_events, wait_events));

  EXPECT_EQ(hCommandList, command_list_initial);
  EXPECT_EQ(hSignalEvent, signal_event_initial);
  for (int i = 0; i < num_wait_events; i++) {
    EXPECT_EQ(wait_events[i], wait_events_initial[i]);
  }
}

void append_barrier(ze_command_list_handle_t cl, ze_event_handle_t hSignalEvent,
                    uint32_t numWaitEvents, ze_event_handle_t *phWaitEvents) {
  auto command_list_initial = cl;
  auto signal_event_initial = hSignalEvent;
  std::vector<ze_event_handle_t> wait_events_initial(numWaitEvents);
  std::memcpy(wait_events_initial.data(), phWaitEvents,
              sizeof(ze_event_handle_t) * numWaitEvents);
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeCommandListAppendBarrier(cl, hSignalEvent, numWaitEvents,
                                       phWaitEvents));
  EXPECT_EQ(cl, command_list_initial);
  EXPECT_EQ(hSignalEvent, signal_event_initial);
  for (int i = 0; i < numWaitEvents; i++) {
    EXPECT_EQ(phWaitEvents[i], wait_events_initial[i]);
  }
}

void append_barrier(ze_command_list_handle_t cl,
                    ze_event_handle_t hSignalEvent) {
  append_barrier(cl, hSignalEvent, 0, nullptr);
}

void append_barrier(ze_command_list_handle_t cl) {
  append_barrier(cl, nullptr, 0, nullptr);
}

void append_memory_ranges_barrier(ze_command_list_handle_t hCommandList,
                                  uint32_t numRanges, const size_t *pRangeSizes,
                                  const void **pRanges,
                                  ze_event_handle_t hSignalEvent,
                                  uint32_t numWaitEvents,
                                  ze_event_handle_t *phWaitEvents) {
  auto command_list_initial = hCommandList;
  auto signal_event_initial = hSignalEvent;
  std::vector<ze_event_handle_t> wait_events_initial(numWaitEvents);
  std::memcpy(wait_events_initial.data(), phWaitEvents,
              sizeof(ze_event_handle_t) * numWaitEvents);
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeCommandListAppendMemoryRangesBarrier(
                hCommandList, numRanges, pRangeSizes, pRanges, hSignalEvent,
                numWaitEvents, phWaitEvents));
  EXPECT_EQ(hCommandList, command_list_initial);
  EXPECT_EQ(hSignalEvent, signal_event_initial);
  for (int i = 0; i < numWaitEvents; i++) {
    EXPECT_EQ(phWaitEvents[i], wait_events_initial[i]);
  }
}

void append_launch_function(ze_command_list_handle_t hCommandList,
                            ze_kernel_handle_t hFunction,
                            const ze_group_count_t *pLaunchFuncArgs,
                            ze_event_handle_t hSignalEvent,
                            uint32_t numWaitEvents,
                            ze_event_handle_t *phWaitEvents) {
  auto command_list_initial = hCommandList;
  auto function_initial = hFunction;
  auto signal_event_initial = hSignalEvent;
  std::vector<ze_event_handle_t> wait_events_initial(numWaitEvents);
  std::memcpy(wait_events_initial.data(), phWaitEvents,
              sizeof(ze_event_handle_t) * numWaitEvents);
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListAppendLaunchKernel(
                                   hCommandList, hFunction, pLaunchFuncArgs,
                                   hSignalEvent, numWaitEvents, phWaitEvents));
  EXPECT_EQ(hCommandList, command_list_initial);
  EXPECT_EQ(hFunction, function_initial);
  EXPECT_EQ(hSignalEvent, signal_event_initial);
  for (int i = 0; i < numWaitEvents; i++) {
    EXPECT_EQ(phWaitEvents[i], wait_events_initial[i]);
  }
}

void append_signal_event(ze_command_list_handle_t hCommandList,
                         ze_event_handle_t hEvent) {
  auto command_list_initial = hCommandList;
  auto event_initial = hEvent;
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeCommandListAppendSignalEvent(hCommandList, hEvent));

  EXPECT_EQ(hCommandList, command_list_initial);
  EXPECT_EQ(hEvent, event_initial);
}

void append_wait_on_events(ze_command_list_handle_t hCommandList,
                           uint32_t numEvents, ze_event_handle_t *phEvents) {
  auto command_list_initial = hCommandList;
  std::vector<ze_event_handle_t> events_initial(numEvents);
  std::memcpy(events_initial.data(), phEvents,
              sizeof(ze_event_handle_t) * numEvents);
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeCommandListAppendWaitOnEvents(hCommandList, numEvents, phEvents));
  EXPECT_EQ(hCommandList, command_list_initial);
  for (int i = 0; i < numEvents; i++) {
    EXPECT_EQ(phEvents[i], events_initial[i]);
  }
}

void query_event(ze_event_handle_t event, ze_result_t result) {
  auto event_initial = event;
  EXPECT_EQ(result, zeEventQueryStatus(event));
  EXPECT_EQ(event, event_initial);
}

void append_reset_event(ze_command_list_handle_t hCommandList,
                        ze_event_handle_t hEvent) {
  auto command_list_initial = hCommandList;
  auto event_initial = hEvent;
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeCommandListAppendEventReset(hCommandList, hEvent));
  EXPECT_EQ(hCommandList, command_list_initial);
  EXPECT_EQ(hEvent, event_initial);
}

void append_image_copy(ze_command_list_handle_t hCommandList,
                       ze_image_handle_t dst, ze_image_handle_t src,
                       ze_event_handle_t hEvent) {

  append_image_copy(hCommandList, dst, src, hEvent, 0, nullptr);
}

void append_image_copy(ze_command_list_handle_t hCommandList,
                       ze_image_handle_t dst, ze_image_handle_t src,
                       ze_event_handle_t hEvent, uint32_t num_wait_events,
                       ze_event_handle_t *wait_events) {
  auto command_list_initial = hCommandList;
  auto dst_initial = dst;
  auto src_initial = src;
  auto signal_event_initial = hEvent;
  std::vector<ze_event_handle_t> wait_events_initial(num_wait_events);
  std::memcpy(wait_events_initial.data(), wait_events,
              sizeof(ze_event_handle_t) * num_wait_events);
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeCommandListAppendImageCopy(hCommandList, dst, src, hEvent,
                                         num_wait_events, wait_events));
  EXPECT_EQ(hCommandList, command_list_initial);
  EXPECT_EQ(hEvent, signal_event_initial);
  EXPECT_EQ(dst, dst_initial);
  EXPECT_EQ(src, src_initial);
  for (int i = 0; i < num_wait_events; i++) {
    EXPECT_EQ(wait_events[i], wait_events_initial[i]);
  }
}

void append_image_copy_to_mem(ze_command_list_handle_t hCommandList, void *dst,
                              ze_image_handle_t src, ze_event_handle_t hEvent) {

  append_image_copy_to_mem(hCommandList, dst, src, hEvent, 0, nullptr);
}

void append_image_copy_to_mem(ze_command_list_handle_t hCommandList, void *dst,
                              ze_image_handle_t src, ze_event_handle_t hEvent,
                              uint32_t num_wait_events,
                              ze_event_handle_t *wait_events) {

  auto command_list_initial = hCommandList;
  auto src_initial = src;
  auto signal_event_initial = hEvent;
  std::vector<ze_event_handle_t> wait_events_initial(num_wait_events);
  std::memcpy(wait_events_initial.data(), wait_events,
              sizeof(ze_event_handle_t) * num_wait_events);
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListAppendImageCopyToMemory(
                                   hCommandList, dst, src, nullptr, hEvent,
                                   num_wait_events, wait_events));
  EXPECT_EQ(hCommandList, command_list_initial);
  EXPECT_EQ(src, src_initial);
  EXPECT_EQ(hEvent, signal_event_initial);
  for (int i = 0; i < num_wait_events; i++) {
    EXPECT_EQ(wait_events[i], wait_events_initial[i]);
  }
}

void append_image_copy_to_mem(ze_command_list_handle_t hCommandList, void *dst,
                              ze_image_handle_t src, ze_image_region_t region,
                              ze_event_handle_t hEvent) {
  append_image_copy_to_mem(hCommandList, dst, src, region, hEvent, 0, nullptr);
}
void append_image_copy_to_mem(ze_command_list_handle_t hCommandList, void *dst,
                              ze_image_handle_t src, ze_image_region_t region,
                              ze_event_handle_t hEvent,
                              uint32_t num_wait_events,
                              ze_event_handle_t *wait_events) {

  auto command_list_initial = hCommandList;
  auto src_initial = src;
  auto signal_event_initial = hEvent;
  std::vector<ze_event_handle_t> wait_events_initial(num_wait_events);
  std::memcpy(wait_events_initial.data(), wait_events,
              sizeof(ze_event_handle_t) * num_wait_events);
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListAppendImageCopyToMemory(
                                   hCommandList, dst, src, &region, hEvent,
                                   num_wait_events, wait_events));
  EXPECT_EQ(hCommandList, command_list_initial);
  EXPECT_EQ(src, src_initial);
  EXPECT_EQ(hEvent, signal_event_initial);
  for (int i = 0; i < num_wait_events; i++) {
    EXPECT_EQ(wait_events[i], wait_events_initial[i]);
  }
}

void append_image_copy_from_mem(ze_command_list_handle_t hCommandList,
                                ze_image_handle_t dst, void *src,
                                ze_event_handle_t hEvent) {

  append_image_copy_from_mem(hCommandList, dst, src, hEvent, 0, nullptr);
}

void append_image_copy_from_mem(ze_command_list_handle_t hCommandList,
                                ze_image_handle_t dst, void *src,
                                ze_image_region_t region,
                                ze_event_handle_t hEvent) {

  append_image_copy_from_mem(hCommandList, dst, src, region, hEvent, 0,
                             nullptr);
}

void append_image_copy_from_mem(ze_command_list_handle_t hCommandList,
                                ze_image_handle_t dst, void *src,
                                ze_event_handle_t hEvent,
                                uint32_t num_wait_events,
                                ze_event_handle_t *wait_events) {

  auto command_list_initial = hCommandList;
  auto dst_initial = dst;
  auto signal_event_initial = hEvent;
  std::vector<ze_event_handle_t> wait_events_initial(num_wait_events);
  std::memcpy(wait_events_initial.data(), wait_events,
              sizeof(ze_event_handle_t) * num_wait_events);
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListAppendImageCopyFromMemory(
                                   hCommandList, dst, src, nullptr, hEvent,
                                   num_wait_events, wait_events));
  EXPECT_EQ(hCommandList, command_list_initial);
  EXPECT_EQ(dst, dst_initial);
  EXPECT_EQ(hEvent, signal_event_initial);
  for (int i = 0; i < num_wait_events; i++) {
    EXPECT_EQ(wait_events[i], wait_events_initial[i]);
  }
}

void append_image_copy_from_mem(ze_command_list_handle_t hCommandList,
                                ze_image_handle_t dst, void *src,
                                ze_image_region_t region,
                                ze_event_handle_t hEvent,
                                uint32_t num_wait_events,
                                ze_event_handle_t *wait_events) {
  auto command_list_initial = hCommandList;
  auto dst_initial = dst;
  auto signal_event_initial = hEvent;
  std::vector<ze_event_handle_t> wait_events_initial(num_wait_events);
  std::memcpy(wait_events_initial.data(), wait_events,
              sizeof(ze_event_handle_t) * num_wait_events);
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListAppendImageCopyFromMemory(
                                   hCommandList, dst, src, &region, hEvent,
                                   num_wait_events, wait_events));
  EXPECT_EQ(hCommandList, command_list_initial);
  EXPECT_EQ(dst, dst_initial);
  EXPECT_EQ(hEvent, signal_event_initial);
  for (int i = 0; i < num_wait_events; i++) {
    EXPECT_EQ(wait_events[i], wait_events_initial[i]);
  }
}

void append_image_copy_region(ze_command_list_handle_t hCommandList,
                              ze_image_handle_t dst, ze_image_handle_t src,
                              const ze_image_region_t *dst_region,
                              const ze_image_region_t *src_region,
                              ze_event_handle_t hEvent) {

  auto command_list_initial = hCommandList;
  auto dst_initial = dst;
  auto src_initial = src;
  auto signal_event_initial = hEvent;

  EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListAppendImageCopyRegion(
                                   hCommandList, dst, src, dst_region,
                                   src_region, hEvent, 0, nullptr));
  EXPECT_EQ(hCommandList, command_list_initial);
  EXPECT_EQ(hEvent, signal_event_initial);
  EXPECT_EQ(dst, dst_initial);
  EXPECT_EQ(src, src_initial);
}

void append_image_copy_region(ze_command_list_handle_t hCommandList,
                              ze_image_handle_t dst, ze_image_handle_t src,
                              const ze_image_region_t *dst_region,
                              const ze_image_region_t *src_region,
                              ze_event_handle_t hEvent,
                              uint32_t num_wait_events,
                              ze_event_handle_t *wait_events) {

  auto command_list_initial = hCommandList;
  auto dst_initial = dst;
  auto src_initial = src;
  auto signal_event_initial = hEvent;
  std::vector<ze_event_handle_t> wait_events_initial(num_wait_events);
  std::memcpy(wait_events_initial.data(), wait_events,
              sizeof(ze_event_handle_t) * num_wait_events);
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeCommandListAppendImageCopyRegion(hCommandList, dst, src,
                                               dst_region, src_region, hEvent,
                                               num_wait_events, wait_events));
  EXPECT_EQ(hCommandList, command_list_initial);
  EXPECT_EQ(hEvent, signal_event_initial);
  EXPECT_EQ(dst, dst_initial);
  EXPECT_EQ(src, src_initial);
  for (int i = 0; i < num_wait_events; i++) {
    EXPECT_EQ(wait_events[i], wait_events_initial[i]);
  }
}

void close_command_list(ze_command_list_handle_t cl) {
  auto command_list_initial = cl;
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListClose(cl));
  EXPECT_EQ(cl, command_list_initial);
}

void reset_command_list(ze_command_list_handle_t cl) {
  auto command_list_initial = cl;
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListReset(cl));
  EXPECT_EQ(cl, command_list_initial);
}

void destroy_command_list(ze_command_list_handle_t cl) {
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListDestroy(cl));
}
}; // namespace level_zero_tests
