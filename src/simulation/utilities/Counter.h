#pragma once

#include "RangeValue.h"

class Counter : public RangeValue<int> {
private:
public:
    Counter(int value_, int maxValue_) : RangeValue(value_, 0, maxValue_) {}

    /// @brief Add 1 to counter value
    /// @return true if value reached max, false otherwise
    bool increment() {
        increase(1);
        return isMax();
    }

    /// @brief Substract 1 from counter value
    /// @return true if value reached 0, false otherwise
    bool decrement() {
        decrease(1);
        return isMin();
    }

    /// @return true if value reached max, false otherwise
    bool isMax() { return get() == getMax(); }
    /// @return true if value reached 0, false otherwise
    bool isMin() { return get() == 0; }
};
