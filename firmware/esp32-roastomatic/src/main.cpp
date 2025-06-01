// MIT License
//
// Copyright (c) Todd Jobe
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// Debugging macro
#define DEBUG 0 // Set to 1 to enable debugging, 0 to disable

#if DEBUG
#define DEBUG_PRINT(x) Serial.println(x)
#define DEBUG_PRINT_VAL(label, value) \
  Serial.print(label);                \
  Serial.println(value)
#else
#define DEBUG_PRINT(x)                // No-op
#define DEBUG_PRINT_VAL(label, value) // No-op
#endif

// Local libraries
#include "Device.h"
#include "ModeRegistry.h"
#include "modes/LoadcellFunction.h"
#include "modes/DisplayFunction.h"
#include "modes/ButtonFunction.h"
#include "modes/ManualRoast.h"
#include "modes/PotentiometerFunction.h"
#include "modes/ThermocoupleFunction.h"

Device &device = Device::getInstance();

void setup()
{
  // Initialize serial communication for debugging
  device.setup();

  // Setup all the modes
  DEBUG_PRINT("Registering modes...");
  ModeRegistry::registerMode("LoadcellFunction", new LoadcellFunction(device));
  DEBUG_PRINT("Registered LoadcellFunction.");
  ModeRegistry::registerMode("DisplayFunction", new DisplayFunction(device));
  DEBUG_PRINT("Registered DisplayFunction.");
  ModeRegistry::registerMode("ButtonFunction", new ButtonFunction(device));
  DEBUG_PRINT("Registered ButtonFunction.");
  ModeRegistry::registerMode("ManualRoast", new ManualRoast(device));
  DEBUG_PRINT("Registered ManualRoast.");
  ModeRegistry::registerMode("PotentiometerFunction", new PotentiometerFunction(device));
  DEBUG_PRINT("Registered PotentiometerFunction.");
  ModeRegistry::registerMode("ThermocoupleFunction", new ThermocoupleFunction(device));
  DEBUG_PRINT("Registered ThermocoupleFunction.");

  // Initial mode setup
  DEBUG_PRINT("Setting initial mode to ManualRoast...");
  device.setMode(ModeRegistry::getMode("ManualRoast"));
  DEBUG_PRINT("Initial mode set.");

  DEBUG_PRINT("Setup complete.");
}

void loop()
{
  // Debugging message for loop execution
  DEBUG_PRINT("Entering loop...");

  // Run the device in the current mode
  device.loop();

  // Debugging message for loop iteration
  DEBUG_PRINT("Exiting loop iteration.");
}