# Task 7: Fixed-Point Arithmetic Analysis

## Overview
This task implements and analyzes fixed-point arithmetic for Mandelbrot set calculations on both STM32F4 and STM32F0 platforms, comparing different scaling factors for precision, overflow risk, and execution speed.

## Implementation Details

### Scaling Factors Tested
- **10^3 (1000)**: 3 decimal places precision
- **10^4 (10000)**: 4 decimal places precision  
- **10^6 (1000000)**: 6 decimal places precision

### Fixed-Point Arithmetic Functions

#### Core Functions
```c
static int64_t fixed_multiply(int64_t a, int64_t b, uint32_t scale);
static int64_t fixed_add(int64_t a, int64_t b);
static int64_t double_to_fixed(double value, uint32_t scale);
static double fixed_to_double(int64_t value, uint32_t scale);
```

#### Mandelbrot Implementation
```c
static uint32_t generate_mandelbrot_checksum_fixed_point(uint16_t width, uint16_t height, uint32_t max_iter, uint32_t scale);
```

### Key Features

#### Overflow Detection
- Monitors for overflow conditions before multiplication operations
- Tracks overflow count per scaling factor
- Breaks iteration when overflow detected

#### Precision Analysis
- Converts floating-point constants to fixed-point representation
- Maintains mathematical accuracy within scaling factor limits
- Compares results across different scaling factors

#### Performance Measurement
- **STM32F4**: Uses DWT cycle counter for precise timing
- **STM32F0**: Uses HAL_GetTick() for timing measurement
- Records execution time and CPU cycles for each test

## Live Expressions Variables

### Global Variables
- `g_current_scaling_factor`: Current scaling factor being tested
- `g_current_scaling_index`: Index of current scaling factor (0-2)
- `g_fixed_overflow_count[0..2]`: Overflow counts for each scaling factor

### Results Arrays
- `g_fixed_checksum[0..2][0..4]`: Checksums for each scaling factor and resolution
- `g_fixed_execution_time_ms[0..2][0..4]`: Execution times for each scaling factor and resolution

## Expected Analysis Results

### Precision Analysis
| Scaling Factor | Decimal Places | Precision Level | Use Case |
|----------------|----------------|-----------------|----------|
| 10^3 (1000)    | 3              | Low             | Fast calculations, rough estimates |
| 10^4 (10000)   | 4              | Medium          | Balanced precision/speed |
| 10^6 (1000000) | 6              | High            | High precision, close to float |

### Overflow Risk Analysis
| Scaling Factor | Overflow Risk | Reason |
|----------------|---------------|---------|
| 10^3 (1000)    | Low           | Smaller numbers, less likely to overflow |
| 10^4 (10000)   | Medium        | Moderate numbers, some overflow possible |
| 10^6 (1000000) | High          | Large numbers, frequent overflow in calculations |

### Execution Speed Analysis
| Scaling Factor | Expected Speed | Reason |
|----------------|----------------|---------|
| 10^3 (1000)    | Fastest        | Smaller numbers, simpler operations |
| 10^4 (10000)   | Medium         | Moderate complexity |
| 10^6 (1000000) | Slowest        | Large numbers, complex multiplication/division |

## Implementation Files Modified

### STM32F0 (Practical_3F0/Core/Src/main.c)
- Added fixed-point global variables
- Implemented fixed-point arithmetic functions
- Added Task 7 analysis to main loop
- Uses HAL_GetTick() for timing

### STM32F4 (Practical_3F4/Core/Src/main.c)
- Added fixed-point global variables
- Implemented fixed-point arithmetic functions
- Added Task 7 analysis to main loop
- Uses DWT cycle counter for precise timing

## Testing Procedure

1. **Build and Flash**: Compile and flash both STM32F0 and STM32F4 projects
2. **Run Analysis**: Execute the program and observe SWO output
3. **Monitor Live Expressions**: Add the global variables to Live Expressions view
4. **Record Results**: Document precision, overflow counts, and execution times
5. **Compare Platforms**: Analyze differences between F0 and F4 performance

## Expected Output Format

```
=== Task 7: Fixed-Point Arithmetic Analysis ===

--- Scaling Factor: 10^3 (1000) ---
Resolution 128x128: 45.123 ms, checksum 1234567, overflows 0
Resolution 160x160: 67.890 ms, checksum 2345678, overflows 0
...

--- Scaling Factor: 10^4 (10000) ---
Resolution 128x128: 48.456 ms, checksum 1234567, overflows 5
...

--- Scaling Factor: 10^6 (1000000) ---
Resolution 128x128: 52.789 ms, checksum 1234567, overflows 25
...

=== Fixed-Point Analysis Summary ===
Scaling Factor 10^3: Total overflows = 0
Scaling Factor 10^4: Total overflows = 15
Scaling Factor 10^6: Total overflows = 67
```

## Analysis Criteria

### Precision Assessment
- Compare checksums across scaling factors
- Identify where precision loss occurs
- Document accuracy vs. floating-point reference

### Overflow Risk Evaluation
- Count overflow occurrences per scaling factor
- Identify problematic calculation regions
- Assess impact on result accuracy

### Performance Comparison
- Measure execution time for each scaling factor
- Compare with floating-point implementation
- Analyze speed vs. precision trade-offs

## Constraints
- **MAX_ITER**: 100 (fixed for all tests)
- **Image Sizes**: Must match Practical 1B sizes (128x128, 160x160, 192x192, 224x224, 256x256)
- **Platforms**: Both STM32F4 and STM32F0 must be tested

## Documentation Requirements
1. **Effect on Precision**: Document how each scaling factor affects calculation accuracy
2. **Risk of Overflow**: Analyze and document overflow patterns for each scaling factor
3. **Execution Speed**: Compare performance across scaling factors and platforms
4. **Trade-off Analysis**: Provide recommendations for scaling factor selection based on use case
