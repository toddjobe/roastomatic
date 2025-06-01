/*
MIT License

© 2025 Todd Jobe

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "Device.h"
#include "Mode.h"

void Device::setup()
{
    DEBUG_PRINT("Device setup started.");

    // Start the Serial
    Serial.begin(115200);
    DEBUG_PRINT("Serial communication initialized.");

    // Initialize the scale
    DEBUG_PRINT("Initializing scale...");
    _scale.setup();
    DEBUG_PRINT("Scale initialized.");

    // Initialize the thermocouples
    DEBUG_PRINT("Initializing thermocouples...");
    _intake_thermocouple.setup();
    _bean_thermocouple.setup();
    DEBUG_PRINT("Thermocouples initialized.");

    // Initialize the buttons
    DEBUG_PRINT("Initializing buttons...");
    for (int i = 0; i < NUM_BUTTONS; i++)
    {
        DEBUG_PRINT_VAL("Setting up button index: ", i);
        _buttons[i].setup();
    }
    DEBUG_PRINT("Buttons initialized.");

    // Run the current mode's setup
    if (_mode)
    {
        DEBUG_PRINT("Running mode setup...");
        _mode->setup();
        DEBUG_PRINT("Mode setup completed.");
    }
    else
    {
        DEBUG_PRINT("No mode set during setup.");
    }

    DEBUG_PRINT("Device setup completed.");
}

void Device::loop()
{
    DEBUG_PRINT("Device loop started.");

    // Read all the sensors at their appropriate rate
    // The potentiometers
    DEBUG_PRINT("Reading potentiometers...");
    _fan_pot.loop();
    _heat_pot.loop();
    DEBUG_PRINT("Potentiometers read.");

    // The scale
    DEBUG_PRINT("Reading scale...");
    _scale.loop();
    DEBUG_PRINT("Scale read.");

    // The thermocouples
    DEBUG_PRINT("Reading thermocouples...");
    _bean_thermocouple.loop();
    _intake_thermocouple.loop();
    DEBUG_PRINT("Thermocouples read.");

    // Check for button presses to change the mode
    DEBUG_PRINT("Checking button 0 for mode change...");
    if (_buttons[0].changed())
    {
        DEBUG_PRINT("Button 0 pressed. Changing mode...");
        _mode = ModeRegistry::getNextMode(_mode);
        if (_mode)
        {
            DEBUG_PRINT("New mode set. Running mode setup...");
            _mode->setup();
            DEBUG_PRINT("Mode setup completed.");
        }
        else
        {
            DEBUG_PRINT("No mode available to set.");
        }
    }

    // Run the current mode's loop
    if (_mode)
    {
        DEBUG_PRINT("Running mode loop...");
        _mode->loop();
        DEBUG_PRINT("Mode loop completed.");
    }
    else
    {
        DEBUG_PRINT("No mode set during loop.");
    }

    DEBUG_PRINT("Device loop completed.");
}

void Device::setMode(Mode *mode)
{
    DEBUG_PRINT("Setting mode...");
    _mode = mode;
    if (_mode)
    {
        DEBUG_PRINT("Running mode setup...");
        _mode->setup();
        DEBUG_PRINT("Mode setup completed.");
    }
    else
    {
        DEBUG_PRINT("No mode provided to set.");
    }
}