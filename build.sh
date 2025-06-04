#!/bin/bash
echo "Building stewy project with PlatformIO..."
cd "$(dirname "$0")"
platformio run
