#pragma once

#include <any>
#include <iostream>
#include "glm/glm.hpp"
#include <vector>

#include "../base.h"

template<typename T>
constexpr size_t getTypePad()
{
        CONSTEXPR_SWITCH(T,
                         CASE(int,       return  4),
                         CASE(float,     return  4),
                         CASE(glm::vec2, return  8),
                         CASE(glm::vec3, return 16),
                         CASE(glm::vec4, return 16),
                         CASE(std::any,  return 0),
        // !std::same_as<T, T> is false, but for some reason in some systems
        // using 'false' causes compilations error.
                         CASE(T, static_assert(!std::same_as<T, T>, "Struct member type padding not implemented."))
        );
        
        return 0;
}

template<typename ...Ts>
constexpr size_t getMaxSize()
{
        return std::max({getTypePad<Ts>()...});
}

template<typename T>
concept HasPadding = requires(T) { T::padding; };

#define $SSBO(name, ...)                                                        \
    struct name {                                                               \
        static constexpr size_t padding = getMaxSize<GET_TYPES(__VA_ARGS__)>(); \
        DECLARE_TYPES(__VA_ARGS__);                                             \
    }

#define GET_TYPES(...) __VA_OPT__(EXPAND(__GET_TYPES(__VA_ARGS__)))

#define __GET_TYPES(type, _, ...)                                               \
    type __VA_OPT__(, __GET_TYPES2 PARENS (__VA_ARGS__))

#define __GET_TYPES2() __GET_TYPES

#define DECLARE_TYPES(...) __VA_OPT__(EXPAND(__DECLARE_TYPES(__VA_ARGS__)))

#define __DECLARE_TYPES(type, name, ...)                                        \
        type name                                                               \
        __VA_OPT__(; __DECLARE_TYPES2 PARENS (__VA_ARGS__))

#define __DECLARE_TYPES2() __DECLARE_TYPES

template <typename T>
struct SSBO {
        static_assert(std::is_fundamental_v<T> || std::same_as<T, std::any> || HasPadding<T>, "SSBO type must be builtin type or created via the $SSBO macro.");
        static constexpr size_t alignment = []() constexpr -> size_t {
                if constexpr (HasPadding<T>)
                        return T::padding;
                else
                        return getTypePad<T>();
        }();
        
        std::vector<T> *const pData;
        
        bool hasSizeAsUniform;
        bool needsUpdate = true;
        
        uint32_t binding;
        uint32_t bufferID{};
        
        SSBO(int binding, std::vector<T> &pData, bool hasSizeAsUniform = true) :
            binding(binding), pData(&pData), hasSizeAsUniform(hasSizeAsUniform) {}
};
