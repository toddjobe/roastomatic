#ifndef FILTER_H
#define FILTER_H

#include <array>

#define DEBUG 1 // Set to 1 to enable debugging, 0 to disable
#if DEBUG
#define DEBUG_PRINT(x) Serial.println(x)
#define DEBUG_PRINT_VAL(label, value) \
    Serial.print(label);              \
    Serial.println(value)
#else
#define DEBUG_PRINT(x)                // No-op
#define DEBUG_PRINT_VAL(label, value) // No-op
#endif

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
        DEBUG_PRINT_VAL("Adding value: ", value);
        buffer[index] = value;             // Store the value in the buffer at the current index
        index = (index + 1) % WINDOW_SIZE; // Move to the next index, wrapping around if necessary

        if (index == 0)
        {                  // Buffer has been completely filled at least once
            filled = true; // Mark the buffer as filled
        }
        DEBUG_PRINT("Buffer");
        for (size_t i = 0; i < WINDOW_SIZE; i++)
        {
            DEBUG_PRINT(buffer[i]); // Debugging: print the current state of the buffer
        }
    }

    // Returns the current filtered value
    T get() const
    {
        size_t count = filled ? WINDOW_SIZE : index; // Use all values if buffer is full, otherwise use available values
        if (count == 0)
            return 0; // Avoid division by zero

        // Incremental averaging to avoid overflow
        double avg = 0.0;
        for (size_t i = 0; i < count; i++)
        {
            avg += (static_cast<double>(buffer[i]) - avg) / (i + 1);
        }
        return static_cast<T>(avg);
    };
};

#endif
