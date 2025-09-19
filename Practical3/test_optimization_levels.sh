#!/bin/bash

# Task 6: Compiler Optimization Analysis Test Script
# This script automates the testing of different optimization levels

echo "=== Task 6: Compiler Optimization Analysis ==="
echo "Testing different optimization levels on STM32F4 and STM32F0"
echo ""

# Function to test optimization level
test_optimization() {
    local project=$1
    local opt_level=$2
    local makefile="Makefile_${opt_level}"
    
    echo "Testing ${project} with ${opt_level} optimization..."
    
    # Change to project directory
    cd "${project}" || exit 1
    
    # Copy the appropriate Makefile
    if [ -f "${makefile}" ]; then
        cp "${makefile}" Makefile
        echo "  Using ${makefile}"
    else
        echo "  ERROR: ${makefile} not found!"
        cd ..
        return 1
    fi
    
    # Clean and build
    echo "  Building..."
    make clean > /dev/null 2>&1
    make all 2>&1 | grep -E "(text|data|bss|dec|hex|filename|error|Error)"
    
    # Check if build was successful
    if [ $? -eq 0 ]; then
        echo "  Build successful!"
        
        # Extract binary size information
        echo "  Binary size information:"
        make all 2>&1 | grep -E "(text|data|bss|dec|hex|filename)" | tail -1
        
        echo "  Ready to flash and test!"
        echo "  Monitor Live Expressions: g_total_program_time_ms"
        echo ""
    else
        echo "  Build failed!"
        echo ""
    fi
    
    # Return to parent directory
    cd ..
}

# Test STM32F4
echo "=== Testing STM32F4 ==="
for opt in O0 O1 O2 O3 Os; do
    test_optimization "Practical_3F4" "${opt}"
done

echo "=== Testing STM32F0 ==="
for opt in O0 O1 O2 O3 Os; do
    test_optimization "Practical_3F0" "${opt}"
done

echo "=== Testing Complete ==="
echo ""
echo "Next steps:"
echo "1. Flash each binary to the respective board"
echo "2. Monitor Live Expressions for g_total_program_time_ms"
echo "3. Record binary sizes from build output"
echo "4. Compare results across optimization levels"
echo ""
echo "Live Expressions to monitor:"
echo "- g_total_program_cycles"
echo "- g_total_program_time_ms"
echo "- checksum[0..4]"
echo "- execution_time_ms[0..4]"
echo "- cpu_cycles[0..4]"
echo "- throughput_pps[0..4]"
