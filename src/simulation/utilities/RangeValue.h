#pragma once

#include <type_traits>
#include <stdexcept>

template <typename T>
class RangeValue
{
    static_assert(std::is_same<T, int>::value ||
                      std::is_same<T, float>::value ||
                      std::is_same<T, double>::value,
                  "Type not allowed!");

private:
    T value;
    T minValue;
    T maxValue;

public:
    
    RangeValue()
        : value(0), minValue(0), maxValue(0) {}
    RangeValue(T value_, T minValue_, T maxValue_)
        : value(value_), minValue(minValue_), maxValue(maxValue_) {}

    /// @brief Use only for ImGui sliders!
    /// @return Pointer to value
    T* valuePointer() { return &value; }

    /// @brief Set newValue to RangeValue.value. Beign cut to [minValue, maxValue]
    /// @param newValue New value of RangeValue
    void set(T newValue) { value = std::max(std::min(newValue, maxValue), minValue); }
    /// @return value of current RangeValue
    T get() { return value; }

    /// @brief Increase current RangeValue.value by amount
    /// @param amount Amount to add to RangeValue.value
    void increase(T amount) { set(value + amount); }

    /// @brief Decrease current RangeValue.value by amount
    /// @param amount Amount to take from RangeValue.value
    void decrease(T amount) { set(value - amount); }

    /// @return Return RangeValue.maxValue
    T getMax() { return maxValue; }

    /// @return Return RangeValue.minValue
    T getMin() { return minValue; }

    /// @brief Set new value for RangeValue.maxValue
    /// @param newMaxValue Value to set
    void setMax(T newMaxValue)
    {
        if (newMaxValue < minValue)
        {
            throw std::invalid_argument("Max value cant be < minValue(" + std::to_string(minValue) + ")");
        }
        maxValue = newMaxValue;
        value = std::min(value, maxValue);
    }
    /// @brief Set new value for RangeValue.minValue
    /// @param newMinValue Value to set
    void setMin(T newMinValue)
    {
        if (newMinValue > maxValue)
        {
            throw std::invalid_argument("Min value cant be > maxValue(" + std::to_string(maxValue) + ")");
        }
        minValue = newMinValue;
        value = std::max(value, minValue);
    }

    /// @return Value in range [0.0, 1.0] that represent current position of value between minValue and maxValue
    float normalize()
    {
        if (minValue == maxValue)
        {
            throw std::invalid_argument("Start and stop cannot be the same value.");
        }
        return (value - minValue) / (maxValue - minValue);
    }
    
};
