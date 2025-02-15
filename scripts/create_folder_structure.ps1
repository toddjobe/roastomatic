# Created the basic structure for this repository

# Set your root directory elsewhere
# [System.Environment]::SetEnvironmentVariable("ProjectRoot", "your\root\directory", [System.EnvironmentVariableTarget]::User)
# Define the root directory
$root = [System.Environment]::GetEnvironmentVariable("ProjectRoot", [System.EnvironmentVariableTarget]::User)

# Define the folder structure
$folders = @(
    "$root/hardware/project1/symbols",
    "$root/hardware/project1/footprints",
    "$root/hardware/project1/gerbers",

    "$root/firmware/esp32-project1/src",
    "$root/firmware/esp32-project1/include",
    "$root/firmware/esp32-project2",  # Another ESP32 project

    "$root/docs/schematics",
    "$root/docs/assembly",
    "$root/docs/software",
    "$root/docs/troubleshooting",

    "$root/scripts",

    "$root/tests/esp32-tests",

    "$root/.github"
)

# Create directories
foreach ($folder in $folders) {
    New-Item -ItemType Directory -Path $folder -Force
}

# Create essential files
$files = @{
    "$root/hardware/project1/project1.kicad_pcb" = ""
    "$root/hardware/project1/project1.kicad_sch" = ""
    "$root/hardware/project1/README.md" = "# Project1 PCB Design"
    "$root/hardware/project1/BOM.csv" = "Part, Quantity, Description, Manufacturer"
    
    "$root/firmware/esp32-project1/platformio.ini" = "[env:esp32dev]`nplatform = espressif32`nboard = esp32dev`nframework = arduino"
    "$root/firmware/esp32-project1/sdkconfig" = "# ESP-IDF Configuration"
    "$root/firmware/esp32-project1/main.cpp" = "#include <Arduino.h>`nvoid setup() { Serial.begin(115200); }`nvoid loop() { Serial.println('Hello, ESP32!'); delay(1000); }"
    "$root/firmware/esp32-project1/README.md" = "# ESP32 Firmware for Project1"

    "$root/docs/overview.md" = "# Project Documentation"
    "$root/docs/README.md" = "# Documentation Overview"

    "$root/scripts/generate_bom.py" = "#!/usr/bin/env python3`n# Script to generate BOM from KiCad files"
    "$root/scripts/upload_firmware.sh" = "#!/bin/bash`necho 'Flashing ESP32...'"
    
    "$root/tests/esp32-tests/test_gpio.cpp" = "#include <unity.h>`nvoid test_gpio() { TEST_ASSERT_TRUE(true); }`nint main() { UNITY_BEGIN(); test_gpio(); UNITY_END(); }"
    "$root/tests/esp32-tests/test_uart.cpp" = "#include <unity.h>`nvoid test_uart() { TEST_ASSERT_TRUE(true); }"
    "$root/tests/esp32-tests/pytest_config.py" = "import pytest"

    "$root/.github/release.yml" = "name: Release Workflow`non: push`njobs: build: runs-on: ubuntu-latest"
    "$root/.github/ci.yml" = "name: CI Workflow`non: push`njobs: build: runs-on: ubuntu-latest"

    "$root/.gitignore" = "build/*`n*.bin`n*.pyc`n__pycache__/"
    "$root/LICENSE" = "MIT License"
    "$root/README.md" = "# Project Repository"
}

# Create files with default content
foreach ($file in $files.Keys) {
    Set-Content -Path $file -Value $files[$file] -Force
}

Write-Host "✅ Project structure created successfully!"
