#ifndef FUNDAMENTAL_H
#define FUNDAMENTAL_H
#include <stdint.h>
#include <stdalign.h>
#include <stdbool.h>
#include <string.h>

// You can change how errors are logged by defining LOG(message) before including this header.
#ifndef LOG
#include <stdio.h>
#define LOG(message) fwrite(message, sizeof(char), sizeof(message) - 1, stderr); fflush(stderr);
#endif

#if defined(NDEBUG)
#define BREAKPOINT() (void)0
#elif defined(_MSC_VER)
#include <intrin.h>
#define BREAKPOINT() __debugbreak()
#elif defined(__GNUC__) || defined(__clang__)
#define BREAKPOINT() __builtin_trap()
#else
#define BREAKPOINT() (void)0
#endif

#if defined(_MSC_VER)
#define RESTRICT __restrict
#elif defined(__GNUC__) || defined(__clang__)
#define RESTRICT __restrict__
#else
#define RESTRICT
#endif

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define LOG_ERROR(message) LOG(__FILE__ ":" TOSTRING(__LINE__) " " message "\n")
#define ERROR_BREAKPOINT(message) LOG_ERROR(message); BREAKPOINT();

#define ASSERT(condition, fallback, message) \
    do { \
        if (!(condition)) { \
            LOG_ERROR("Assertion failed: " #message); \
            BREAKPOINT(); \
            fallback; \
        } \
    } while (0)

#ifndef NDEBUG
#define DEBUG_ASSERT(condition, fallback, message) ASSERT(condition, fallback, message)
#else
#define DEBUG_ASSERT(condition, fallback, message) (void)0
#endif

#define STATIC_ASSERT(condition, message) \
    typedef char static_assertion_##message[(condition) ? 1 : -1];

typedef enum {
    RESULT_FAILURE,
    RESULT_SUCCESS
} result;

#define DECLARE_CAPPED_ARRAY(element_type, name, capacity) \
    typedef struct { \
        element_type data[capacity]; \
        uint32_t count; \
    } name; \
    result name##_append(name* array, element_type value); \
    result name##_remove(name* array, uint32_t index); \
    result name##_remove_swap(name* array, uint32_t index); \
    bool name##_contains(const name* array, element_type value); \
    static inline result name##_get(const name* array, uint32_t index, element_type* out) { \
        ASSERT(index < array->count, return RESULT_FAILURE, "Index out of bounds"); \
        memcpy(out, &array->data[index], sizeof(element_type)); \
        return RESULT_SUCCESS; \
    } \
    static inline result name##_set(name* array, uint32_t index, element_type value) { \
        ASSERT(index < array->count, return RESULT_FAILURE, "Index out of bounds"); \
        memcpy(&array->data[index], &value, sizeof(element_type)); \
        return RESULT_SUCCESS; \
    } \

#define IMPLEMENT_CAPPED_ARRAY(element_type, name, capacity) \
    STATIC_ASSERT(sizeof(name) == sizeof(struct { element_type data[capacity]; uint32_t count; }), name##_declaration_size_mismatch) \
    result name##_append(name* array, element_type value) { \
        if (array->count < capacity) { \
            memcpy(&array->data[array->count++], &value, sizeof(element_type)); \
            return RESULT_SUCCESS; \
        } \
        ERROR_BREAKPOINT("Array capacity exceeded"); \
        return RESULT_FAILURE; \
    } \
    result name##_remove(name* array, uint32_t index) { \
        if (index < array->count) { \
            memmove(&array->data[index], &array->data[index + 1], (array->count - index - 1) * sizeof(element_type)); \
            --array->count; \
            return RESULT_SUCCESS; \
        } \
        ERROR_BREAKPOINT("Index out of bounds"); \
        return RESULT_FAILURE; \
    } \
    result name##_remove_swap(name* array, uint32_t index) { \
        if (index < array->count) { \
            memcpy(&array->data[index], &array->data[array->count - 1], sizeof(element_type)); \
            --array->count; \
            return RESULT_SUCCESS; \
        } \
        ERROR_BREAKPOINT("Index out of bounds"); \
        return RESULT_FAILURE; \
    } \
    bool name##_contains(const name* array, element_type value) { \
        for (uint32_t i = 0; i < array->count; ++i) { \
            if (memcmp(&array->data[i], &value, sizeof(element_type)) == 0) { \
                return true; \
            } \
        } \
        return false; \
    }

#endif // FUNDAMENTAL_H