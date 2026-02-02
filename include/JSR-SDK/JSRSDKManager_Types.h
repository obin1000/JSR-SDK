/**
 * @file JSRSDKManager_Types.h
 * @brief Type definitions for JSR SDK C API
 * 
 * This header provides C-compatible type definitions used by both C and C++ APIs.
 * All types are designed to be safe across DLL boundaries using fixed-size buffers
 * and no dynamic memory allocation.
 */

#pragma once

#ifndef JSRSDKMANAGER_TYPES_H
#define JSRSDKMANAGER_TYPES_H

#include "JSR-SDK/events/StatusChangedEvent.h"
#include "JSR-SDK/events/NotifyEvent.h"

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// Opaque Handle Types
// ============================================================================

/** @brief Opaque handle to the SDK manager instance */
typedef void* JSRSDKManagerHandle;

// ============================================================================
// Callback Function Types
// ============================================================================

/**
 * @brief Callback invoked when device status changes
 * @param evt Pointer to the status change event (C-compatible structure)
 * @param user_data User-provided context pointer
 */
typedef void (*JSR_StatusChangeCallback)(const StatusChangedEvent* evt, void* user_data);

/**
 * @brief Callback invoked for manager notifications
 * @param evt Pointer to the notify event (C-compatible structure)
 * @param user_data User-provided context pointer
 */
typedef void (*JSR_NotifyCallback)(const NotifyEvent* evt, void* user_data);

#ifdef __cplusplus
}
#endif

#endif // JSRSDKMANAGER_TYPES_H
