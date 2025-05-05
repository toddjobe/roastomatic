#ifndef FILTER_H
#define FILTER_H

#include <array>

// Template class for a simple moving average filter
template <typename T, size_t WINDOW_SIZE>
class Filter
{
private:
    std::array<T, WINDOW_SIZE> buffer = {}; // Circular buffer initialized with zeros
    size_t index = 0;                       // Current index in the circular buffer
    bool filled = false;                    // Indicates if the buffer has enough samples to be considered "filled"

public:
    // Adds a new value to the filter
    void add(T value)
    {
        buffer[index] = value;             // Store the value in the buffer at the current index
        index = (index + 1) % WINDOW_SIZE; // Move to the next index, wrapping around if necessary

        if (index == 0)
        {                  // Buffer has been completely filled at least once
            filled = true; // Mark the buffer as filled
        }
    }

    // Returns the current filtered value
    T get() const
    {
        size_t count = filled ? WINDOW_SIZE : index; // Use all values if buffer is full, otherwise use available values
        if (count == 0)
            return 0; // Avoid division by zero

        T sum = 0; // Initialize sum to zero
        for (size_t i = 0; i < count; i++)
        {
            sum += buffer[i]; // Sum all values in the buffer
        }
        return sum / count; // Return the average of the values
    };
};

#endif
