# Task 6: Compiler Optimization Analysis

## Overview
This document describes the implementation and analysis of compiler optimization impact on STM32F4 and STM32F0 Mandelbrot calculations, measuring binary size and runtime performance across different optimization levels.

## Implementation Details

### Test Configuration
- **Platforms**: STM32F446 (F4) and STM32F051 (F0)
- **MAX_ITER**: 100 (fixed)
- **Image Sizes**: 128×128, 160×160, 192×192, 224×224, 256×256 (Practical 1B sizes)
- **Optimization Levels**: O0, O1, O2, O3, Os

### Optimization Levels Tested

#### **O0 (No Optimization)**
- **Description**: No optimization, fastest compilation
- **Characteristics**: Largest binary size, slowest execution
- **Use Case**: Debugging, development

#### **O1 (Basic Optimization)**
- **Description**: Basic optimizations, faster compilation than O2
- **Characteristics**: Moderate size reduction, moderate speed improvement
- **Use Case**: Development with some optimization

#### **O2 (Standard Optimization)**
- **Description**: Standard optimizations, good balance
- **Characteristics**: Good size reduction, good speed improvement
- **Use Case**: Production builds (recommended)

#### **O3 (Aggressive Optimization)**
- **Description**: Aggressive optimizations, may increase binary size
- **Characteristics**: May be larger than O2, potentially fastest execution
- **Use Case**: Performance-critical applications

#### **Os (Size Optimization)**
- **Description**: Optimize for size, not speed
- **Characteristics**: Smallest binary size, slower execution
- **Use Case**: Memory-constrained applications

## Makefile Configurations

### STM32F4 Makefiles
- `Makefile_O0` - No optimization
- `Makefile_O1` - Basic optimization
- `Makefile_O2` - Standard optimization
- `Makefile_O3` - Aggressive optimization
- `Makefile_Os` - Size optimization

### STM32F0 Makefiles
- `Makefile_O0` - No optimization
- `Makefile_O1` - Basic optimization
- `Makefile_O2` - Standard optimization
- `Makefile_O3` - Aggressive optimization
- `Makefile_Os` - Size optimization

## Code Implementation

### Key Features
1. **Total Program Timing**: Measures entire program execution time
2. **Per-Resolution Timing**: Individual test timing for each image size
3. **Live Expressions**: Real-time monitoring of performance metrics
4. **Comprehensive Logging**: Detailed output for analysis

### Live Expressions Available

#### **Performance Metrics**
- `g_total_program_cycles` - Total CPU cycles for entire program
- `g_total_program_time_ms` - Total execution time in milliseconds
- `g_current_cycles` - Current test CPU cycles
- `g_current_execution_time` - Current test execution time
- `g_current_throughput` - Current test throughput (pixels/second)

#### **Per-Resolution Results**
- `checksum[0..4]` - Mandelbrot checksums for each resolution
- `execution_time_ms[0..4]` - Execution times for each resolution
- `cpu_cycles[0..4]` - CPU cycles for each resolution
- `throughput_pps[0..4]` - Throughput for each resolution

#### **Configuration**
- `g_optimization_level` - Current optimization level (0=O0, 1=O1, 2=O2, 3=O3, 4=Os)

## Testing Procedure

### Step 1: Test O0 (No Optimization)
```bash
cd Practical_3F4  # or Practical_3F0
cp Makefile_O0 Makefile
make clean && make all
# Flash and run, record:
# - Binary size (from build output)
# - Total program time (from Live Expressions: g_total_program_time_ms)
```

### Step 2: Test O1 (Basic Optimization)
```bash
cp Makefile_O1 Makefile
make clean && make all
# Flash and run, record results
```

### Step 3: Test O2 (Standard Optimization)
```bash
cp Makefile_O2 Makefile
make clean && make all
# Flash and run, record results
```

### Step 4: Test O3 (Aggressive Optimization)
```bash
cp Makefile_O3 Makefile
make clean && make all
# Flash and run, record results
```

### Step 5: Test Os (Size Optimization)
```bash
cp Makefile_Os Makefile
make clean && make all
# Flash and run, record results
```

## Expected Results Analysis

### Binary Size Trends

#### **STM32F4 (Expected)**
| Optimization | Binary Size (KB) | Size vs O0 |
|--------------|------------------|------------|
| O0           | ~180            | 100%       |
| O1           | ~150            | 83%        |
| O2           | ~140            | 78%        |
| O3           | ~145            | 81%        |
| Os           | ~120            | 67%        |

#### **STM32F0 (Expected)**
| Optimization | Binary Size (KB) | Size vs O0 |
|--------------|------------------|------------|
| O0           | ~80             | 100%       |
| O1           | ~65             | 81%        |
| O2           | ~60             | 75%        |
| O3           | ~62             | 78%        |
| Os           | ~50             | 63%        |

### Runtime Performance Trends

#### **STM32F4 (Expected)**
| Optimization | Total Time (ms) | Speedup vs O0 |
|--------------|-----------------|---------------|
| O0           | ~2000          | 1.0x          |
| O1           | ~1200          | 1.7x          |
| O2           | ~800           | 2.5x          |
| O3           | ~750           | 2.7x          |
| Os           | ~1000          | 2.0x          |

#### **STM32F0 (Expected)**
| Optimization | Total Time (ms) | Speedup vs O0 |
|--------------|-----------------|---------------|
| O0           | ~8000          | 1.0x          |
| O1           | ~5000          | 1.6x          |
| O2           | ~3500          | 2.3x          |
| O3           | ~3200          | 2.5x          |
| Os           | ~4500          | 1.8x          |

## Analysis Framework

### Performance Metrics to Record

#### **Binary Size Analysis**
1. **Flash Usage**: Total program size in Flash memory
2. **RAM Usage**: Static and global variable usage
3. **Size Reduction**: Percentage reduction compared to O0

#### **Runtime Analysis**
1. **Total Execution Time**: Complete program runtime
2. **Per-Resolution Timing**: Individual test execution times
3. **Throughput**: Pixels processed per second
4. **Speedup Factor**: Performance improvement over O0

### Comparison Table Template

| Optimization | Binary Size (KB) | Total Time (ms) | Speedup vs O0 | Size vs O0 |
|--------------|------------------|-----------------|---------------|------------|
| O0           | [Record]         | [Record]        | 1.0x          | 100%       |
| O1           | [Record]         | [Record]        | [Calculate]   | [Calculate]|
| O2           | [Record]         | [Record]        | [Calculate]   | [Calculate]|
| O3           | [Record]         | [Record]        | [Calculate]   | [Calculate]|
| Os           | [Record]         | [Record]        | [Calculate]   | [Calculate]|

## Key Findings Expected

### **STM32F4 (Cortex-M4 with FPU)**
1. **O2 provides best balance** of size and performance
2. **O3 may provide fastest execution** but larger binary
3. **Os provides smallest binary** but slower execution
4. **FPU acceleration** makes optimization impact more pronounced

### **STM32F0 (Cortex-M0 without FPU)**
1. **O2 provides best balance** of size and performance
2. **Software floating-point** makes optimization impact significant
3. **Os provides smallest binary** but noticeable performance penalty
4. **Memory constraints** make size optimization important

### **General Trends**
1. **O0**: Baseline (largest, slowest)
2. **O1**: Moderate improvements
3. **O2**: Best overall balance (recommended for production)
4. **O3**: May be fastest but larger
5. **Os**: Smallest but slower

## Optimization Trade-offs

### **Size vs Speed**
- **O2**: Good balance, recommended for most applications
- **O3**: Optimize for speed, accept larger binary
- **Os**: Optimize for size, accept slower execution

### **Development vs Production**
- **O0**: Use during development and debugging
- **O2**: Use for production builds
- **O3/Os**: Use for specific performance/size requirements

### **Platform Considerations**
- **STM32F4**: FPU makes optimization impact more significant
- **STM32F0**: Software floating-point makes optimization crucial
- **Memory constraints**: Size optimization may be necessary

## Conclusion

This analysis demonstrates the significant impact of compiler optimization on embedded system performance. The results will show:

1. **Performance**: Optimization can provide 2-3x speedup
2. **Size**: Optimization can reduce binary size by 20-40%
3. **Trade-offs**: Developers must balance speed vs size based on requirements
4. **Platform differences**: FPU vs software floating-point affects optimization impact

The implementation provides comprehensive monitoring through Live Expressions, enabling real-time analysis of optimization impact on Mandelbrot calculations across different optimization levels and platforms.
