#ifndef HASH_SPECIALIZATIONS_H
#define HASH_SPECIALIZATIONS_H

#include <memory>
#include <unordered_set>

namespace std {
    template <typename T>
    struct hash<std::weak_ptr<T>> {
        size_t operator()(const std::weak_ptr<T>& wp) const {
            auto shared = wp.lock();
            return shared ? reinterpret_cast<size_t>(shared.get()) : 0;
        }
    };

    template <typename T>
    struct equal_to<std::weak_ptr<T>> {
        bool operator()(const std::weak_ptr<T>& wp1, const std::weak_ptr<T>& wp2) const {
            return wp1.lock() == wp2.lock();
        }
    };
}

#endif // HASH_SPECIALIZATIONS_H