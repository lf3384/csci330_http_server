#!/bin/bash

echo "Compiling C++ HTTP Server (Phase 4)..."
echo "======================================="

# Compile all source files
g++ -std=c++11 -Wall -Wextra -o server \
    main.cpp \
    ConfigManager.cpp \
    HTTPRequest.cpp \
    HTTPResponse.cpp \
    FileHandler.cpp \
    HTTPServer.cpp \
    Logger.cpp

if [ $? -eq 0 ]; then
    echo "✓ Compilation successful!"
    echo ""
    echo "To run the server:"
    echo "  ./server"
    echo ""
    echo "Or with custom config:"
    echo "  ./server my_config.ini"
else
    echo "✗ Compilation failed!"
    exit 1
fi
