#pragma once

namespace CraftBlock {
    template<bool B, typename T> struct cond {};

    template<typename T> struct cond<true, T> {
        using type = T;
    };

    template<typename T> struct cond<false, T> {
        using type = struct {
            [[deprecated("Field doesn't exist!")]]
            operator float() {
                return 0;
            }

            [[deprecated("Field doesn't exist!")]]
            float operator=(float) {
                return 0;
            }
        };
    };

    template<bool B, typename T> using cond_t = typename cond<B, T>::type;
}