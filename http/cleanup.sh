#!/bin/bash

echo "Cleaning up project directory..."
echo "================================="

cd "$(dirname "$0")"

# Remove compiled binaries
echo "Removing compiled binaries..."
rm -f server main *.o

# Remove test/scratch files
echo "Removing test files..."
rm -f test.cpp hello.html

# Remove old phase documentation (merged into main README)
echo "Removing redundant documentation..."
rm -f PHASE4_README.md

# Remove log files (they'll be regenerated)
echo "Removing old log files..."
rm -f server.log *.log

echo ""
echo "✓ Cleanup complete!"
echo ""
echo "Removed:"
echo "  - Compiled binaries (server, main, *.o)"
echo "  - Test files (test.cpp, hello.html)"
echo "  - Old documentation (PHASE4_README.md)"
echo "  - Log files (server.log)"
echo ""
echo "To rebuild the server, run:"
echo "  ./compile.sh"
