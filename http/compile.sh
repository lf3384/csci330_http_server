#!/bin/bash

echo "==================================="
echo "Compiling C++ HTTP Server..."
echo "==================================="

# Compile with warnings enabled
g++ -std=c++17 -Wall -Wextra -o main main.cpp

# Check if compilation succeeded
if [ $? -eq 0 ]; then
    echo "✓ Compilation successful!"
    echo ""
    echo "Run the server with: ./main"
    echo "Then visit: http://localhost:8080"
else
    echo "✗ Compilation failed!"
    exit 1
fi
