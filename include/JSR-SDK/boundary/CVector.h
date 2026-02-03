#pragma once

#include <cstring>

#ifdef __cplusplus
#include <vector>
#endif

/**
 * @brief ABI-safe vector representation for crossing DLL boundaries.
 * 
 * This struct uses only C-compatible types (pointer, size, capacity)
 * which have the same binary representation across different compilers.
 * 
 * Memory is owned by the DLL that creates it and must be freed using
 * the same DLL's memory allocator.
 * 
 * Note: This is a C++ template and cannot be used directly from C code.
 * C code can work with specific instantiations if proper typedefs are provided.
 * 
 * @tparam T The element type (must be a POD type or another ABI-safe type)
 */
#ifdef __cplusplus
template <typename T> struct CVector {
  T *data;
  size_t size;
  size_t capacity;

  CVector() : data(nullptr), size(0), capacity(0) {}

  CVector(T *ptr, size_t sz, size_t cap)
      : data(ptr), size(sz), capacity(cap) {}

  /**
   * @brief Convert std::vector to CVector (allocates memory).
   * Memory must be freed by calling free_cvector().
   */
  static CVector<T> from_std_vector(const std::vector<T> &vec) {
    if (vec.empty()) {
      return CVector<T>();
    }
    T *buffer = new T[vec.size()];
    std::memcpy(buffer, vec.data(), vec.size() * sizeof(T));
    return CVector<T>(buffer, vec.size(), vec.size());
  }

  /**
   * @brief Convert CVector to std::vector (copies data).
   */
  std::vector<T> to_std_vector() const {
    if (data == nullptr || size == 0) {
      return std::vector<T>();
    }
    return std::vector<T>(data, data + size);
  }

  /**
   * @brief Free memory allocated by from_std_vector().
   */
  void free_cvector() {
    if (data != nullptr) {
      delete[] data;
      data = nullptr;
      size = 0;
      capacity = 0;
    }
  }

  /**
   * @brief Access element at index (no bounds checking).
   */
  T &operator[](size_t index) { return data[index]; }

  const T &operator[](size_t index) const { return data[index]; }
};
#endif
