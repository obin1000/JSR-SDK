#pragma once

#include <cstring>

#ifdef __cplusplus
#include <string>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ABI-safe string representation for crossing DLL boundaries.
 * 
 * This struct uses only C-compatible types (char pointer and size)
 * which have the same binary representation across different compilers.
 * 
 * Memory is owned by the DLL that creates it and must be freed using
 * the same DLL's memory allocator.
 */
struct CString {
  const char *data;
  size_t length;

#ifdef __cplusplus
  CString() : data(nullptr), length(0) {}

  CString(const char *str, size_t len) : data(str), length(len) {}

  /**
   * @brief Convert std::string to CString (allocates memory).
   * Memory must be freed by calling free_cstring().
   */
  static CString from_std_string(const std::string &str) {
    if (str.empty()) {
      return CString();
    }
    char *buffer = new char[str.size() + 1];
    std::memcpy(buffer, str.data(), str.size());
    buffer[str.size()] = '\0';
    return CString(buffer, str.size());
  }

  /**
   * @brief Convert CString to std::string (copies data).
   */
  std::string to_std_string() const {
    if (data == nullptr || length == 0) {
      return std::string();
    }
    return std::string(data, length);
  }

  /**
   * @brief Free memory allocated by from_std_string().
   */
  void free_cstring() {
    if (data != nullptr) {
      delete[] data;
      data = nullptr;
      length = 0;
    }
  }
#endif
};

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
/**
 * @brief Free a CString that was allocated by the DLL.
 */
inline void free_cstring(CString &str) { str.free_cstring(); }
#endif
