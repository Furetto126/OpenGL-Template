#pragma once

#include <any>
#include <iostream>
#include "glm/glm.hpp"
#include <vector>

namespace AppGL::macros
{
        #define PARENS ()
        #define EXPAND(...) EXPAND4(EXPAND4(EXPAND4(EXPAND4(__VA_ARGS__))))
        #define EXPAND4(...) EXPAND3(EXPAND3(EXPAND3(EXPAND3(__VA_ARGS__))))
        #define EXPAND3(...) EXPAND2(EXPAND2(EXPAND2(EXPAND2(__VA_ARGS__))))
        #define EXPAND2(...) EXPAND1(EXPAND1(EXPAND1(EXPAND1(__VA_ARGS__))))
        #define EXPAND1(...) __VA_ARGS__
        
        #define CASE(type, ...) type, { __VA_ARGS__; }
        
        #define CONSTEXPR_SWITCH(var, ...)                                              \
                do {                                                                    \
                        __VA_OPT__(EXPAND(__CONSTEXPR_SWITCH(var, __VA_ARGS__)))        \
                } while (false)
        
        // The actual macro definition.
        #define __CONSTEXPR_SWITCH(var, case1, codeBlock1, ...)                         \
                if constexpr (std::same_as<var, case1>)                                 \
                        codeBlock1                                                      \
                __VA_OPT__(else __CONSTEXPR_SWITCH2 PARENS (var, __VA_ARGS__))
        
        #define __CONSTEXPR_SWITCH2() __CONSTEXPR_SWITCH
}