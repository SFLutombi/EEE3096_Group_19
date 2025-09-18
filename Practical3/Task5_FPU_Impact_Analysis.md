# Task 5: FPU Impact Analysis

## Overview
This document describes the implementation and analysis of FPU (Floating-Point Unit) impact on STM32F4 Mandelbrot calculations, comparing performance and accuracy between float and double precision with FPU enabled and disabled.

## Implementation Details

### Test Configuration
- **Platform**: STM32F446 (Cortex-M4 with FPU)
- **MAX_ITER**: 100 (fixed)
- **Image Sizes**: 128×128, 160×160, 192×192, 224×224, 256×256 (Practical 1B sizes)
- **Precision Types**: Float (32-bit) and Double (64-bit)
- **FPU States**: Enabled and Disabled

### Makefile Configurations

#### FPU Enabled Configuration
```makefile
# FPU ENABLED
FPU = -mfpu=fpv4-sp-d16
FLOAT-ABI = -mfloat-abi=hard
```

#### FPU Disabled Configuration
```makefile
# FPU DISABLED
FPU = 
FLOAT-ABI = -mfloat-abi=soft
```

### Code Implementation

#### Float Precision Function
```c
static uint32_t generate_mandelbrot_checksum_float(uint16_t width, uint16_t height, uint32_t max_iter)
{
    uint32_t mandelbrot_sum = 0u;
    for (uint16_t y = 0; y < height; ++y)
    {
        float y0 = ((float)y / (float)height) * 2.0f - 1.0f;
        for (uint16_t x = 0; x < width; ++x)
        {
            float x0 = ((float)x / (float)width) * 3.5f - 2.5f;
            float xi = 0.0f;
            float yi = 0.0f;
            uint32_t iteration = 0u;
            while (iteration < max_iter && (xi*xi + yi*yi) <= 4.0f)
            {
                float tmp = xi*xi - yi*yi + x0;
                yi = 2.0f*xi*yi + y0;
                xi = tmp;
                ++iteration;
            }
            mandelbrot_sum += iteration;
        }
    }
    return mandelbrot_sum;
}
```

#### Double Precision Function
```c
static uint32_t generate_mandelbrot_checksum_double(uint16_t width, uint16_t height, uint32_t max_iter)
{
    uint32_t mandelbrot_sum = 0u;
    for (uint16_t y = 0; y < height; ++y)
    {
        double y0 = ((double)y / (double)height) * 2.0 - 1.0;
        for (uint16_t x = 0; x < width; ++x)
        {
            double x0 = ((double)x / (double)width) * 3.5 - 2.5;
            double xi = 0.0;
            double yi = 0.0;
            uint32_t iteration = 0u;
            while (iteration < max_iter && (xi*xi + yi*yi) <= 4.0)
            {
                double tmp = xi*xi - yi*yi + x0;
                yi = 2.0*xi*yi + y0;
                xi = tmp;
                ++iteration;
            }
            mandelbrot_sum += iteration;
        }
    }
    return mandelbrot_sum;
}
```

## Live Expressions for Monitoring

### Float Precision Results
- `checksum_float[0..4]` - Mandelbrot checksums using float precision
- `execution_time_ms_float[0..4]` - Execution times in milliseconds
- `cpu_cycles_float[0..4]` - CPU cycle counts
- `throughput_pps_float[0..4]` - Throughput in pixels per second

### Double Precision Results
- `checksum_double[0..4]` - Mandelbrot checksums using double precision
- `execution_time_ms_double[0..4]` - Execution times in milliseconds
- `cpu_cycles_double[0..4]` - CPU cycle counts
- `throughput_pps_double[0..4]` - Throughput in pixels per second

### Comparison Metrics
- `accuracy_diff_percent[0..4]` - Accuracy difference percentage between float and double
- `speedup_factor[0..4]` - Speedup factor (double time / float time)
- `g_current_test_mode` - Current test mode (0=float, 1=double)
- `g_fpu_enabled` - FPU status (1=enabled, 0=disabled)

## Expected Results Analysis

### FPU Impact on Performance

#### With FPU Enabled
- **Float Operations**: Hardware-accelerated, single-cycle operations
- **Double Operations**: Hardware-accelerated, but may require multiple cycles
- **Performance**: Significantly faster than software emulation

#### With FPU Disabled
- **Float Operations**: Software emulation, multiple cycles per operation
- **Double Operations**: Software emulation, many cycles per operation
- **Performance**: Much slower, especially for double precision

### Accuracy Differences

#### Float vs Double Precision
- **Float (32-bit)**: ~7 decimal digits of precision
- **Double (64-bit)**: ~15 decimal digits of precision
- **Mandelbrot Impact**: Accumulated rounding errors in float precision
- **Expected Difference**: Small percentage difference in checksums

### Speedup Analysis

#### Float vs Double (FPU Enabled)
- **Expected Speedup**: Float should be 1.5-2x faster than double
- **Reason**: Double operations may require multiple FPU cycles

#### FPU Enabled vs Disabled
- **Expected Speedup**: 10-50x faster with FPU enabled
- **Reason**: Hardware acceleration vs software emulation

## Testing Procedure

### Step 1: Build with FPU Enabled
```bash
cd Practical_3F4
cp Makefile_FPU_ENABLED Makefile
make clean
make all
```

### Step 2: Flash and Run
- Flash the binary to STM32F4
- Monitor Live Expressions in STM32CubeIDE
- Record results for both float and double precision

### Step 3: Build with FPU Disabled
```bash
cp Makefile_FPU_DISABLED Makefile
make clean
make all
```

### Step 4: Flash and Run Again
- Flash the new binary
- Monitor Live Expressions
- Record results for comparison

### Step 5: Analysis
- Compare execution times
- Calculate speedup factors
- Analyze accuracy differences
- Document findings

## Expected Performance Characteristics

### Execution Time Comparison (Estimated)
| Resolution | Float (FPU) | Double (FPU) | Float (No FPU) | Double (No FPU) |
|------------|-------------|--------------|----------------|-----------------|
| 128×128    | ~50ms       | ~75ms        | ~500ms         | ~2000ms         |
| 256×256    | ~200ms      | ~300ms       | ~2000ms        | ~8000ms         |

### Accuracy Comparison
- **Float vs Double Checksum Difference**: 0.1% - 2%
- **Higher resolutions**: Larger accuracy differences due to accumulated errors

### Speedup Factors
- **Float vs Double (FPU)**: 1.5x - 2x
- **FPU vs No FPU (Float)**: 10x - 20x
- **FPU vs No FPU (Double)**: 20x - 50x

## Key Findings Expected

1. **FPU Impact**: Massive performance improvement with FPU enabled
2. **Precision Trade-off**: Float is faster but less accurate than double
3. **Resolution Scaling**: Performance differences become more pronounced at higher resolutions
4. **Memory Usage**: Double precision uses more memory but provides better accuracy

## Conclusion

This analysis demonstrates the significant impact of the FPU on floating-point performance in embedded systems. The results will show:

- **Performance**: FPU provides substantial speedup for floating-point operations
- **Accuracy**: Double precision provides better accuracy at the cost of performance
- **Trade-offs**: Developers must balance speed vs accuracy based on application requirements

The implementation provides comprehensive monitoring through Live Expressions, enabling real-time analysis of FPU impact on Mandelbrot calculations across different precisions and configurations.
