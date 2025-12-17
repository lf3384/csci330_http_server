# Makefile for C++ HTTP Server - Phase 4

CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic
TARGET = server
SOURCES = main.cpp ConfigManager.cpp HTTPRequest.cpp HTTPResponse.cpp FileHandler.cpp HTTPServer.cpp
OBJECTS = $(SOURCES:.cpp=.o)
HEADERS = ConfigManager.h HTTPRequest.h HTTPResponse.h FileHandler.h HTTPServer.h

# Default target
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJECTS)
	@echo "Linking $(TARGET)..."
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)
	@echo "✓ Build successful!"
	@echo ""
	@echo "Run with: ./$(TARGET)"

# Compile source files to object files
%.o: %.cpp $(HEADERS)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	rm -f $(OBJECTS) $(TARGET)
	@echo "✓ Clean complete"

# Rebuild from scratch
rebuild: clean all

# Run the server
run: $(TARGET)
	./$(TARGET)

# Show help
help:
	@echo "Available targets:"
	@echo "  make        - Build the server"
	@echo "  make clean  - Remove build artifacts"
	@echo "  make rebuild - Clean and rebuild"
	@echo "  make run    - Build and run server"
	@echo "  make help   - Show this help"

.PHONY: all clean rebuild run help
