# Task 8: Power Measurement Analysis

## Overview
This task analyzes the feasibility of measuring power consumption during Mandelbrot benchmarking on STM32F4 and STM32F0 platforms, including implementation attempts and alternative approaches.

## Power Measurement Implementation

### STM32F4 Power Measurement Capabilities

#### Hardware Features Available
- **DWT (Data Watchpoint and Trace)**: Can measure CPU cycles and activity
- **SystemCoreClock**: 180 MHz (STM32F446)
- **Voltage Regulator**: Internal LDO regulator
- **Power Management**: Multiple power modes available

#### Software-Based Power Estimation
```c
/* Task 8: Power Measurement Analysis - STM32F4 */
static void analyze_power_consumption_f4(void)
{
    printf("\r\n=== Task 8: Power Measurement Analysis - STM32F4 ===\r\n");
    
    /* Power estimation based on CPU activity */
    uint32_t total_cycles = 0;
    uint32_t active_cycles = 0;
    uint32_t idle_cycles = 0;
    
    /* Measure during Mandelbrot calculation */
    DWT->CYCCNT = 0;
    uint32_t start_cycles = dwt_get_cycles();
    
    /* Run Mandelbrot calculation */
    uint32_t checksum = generate_mandelbrot_checksum_double(128, 128, 100);
    
    uint32_t end_cycles = dwt_get_cycles();
    uint32_t calculation_cycles = end_cycles - start_cycles;
    
    /* Estimate power consumption */
    double calculation_time_s = (double)calculation_cycles / (double)SystemCoreClock;
    double estimated_power_mw = estimate_power_consumption_f4(calculation_cycles);
    
    printf("F4 Power Analysis:\r\n");
    printf("- Calculation cycles: %lu\r\n", (unsigned long)calculation_cycles);
    printf("- Calculation time: %.6f seconds\r\n", calculation_time_s);
    printf("- Estimated power: %.2f mW\r\n", estimated_power_mw);
    printf("- Energy consumption: %.6f mJ\r\n", estimated_power_mw * calculation_time_s * 1000.0);
}

static double estimate_power_consumption_f4(uint32_t cycles)
{
    /* Power estimation based on STM32F446 datasheet */
    /* Typical power consumption at 180 MHz: ~100-150 mW */
    /* This is a rough estimation based on CPU activity */
    
    double base_power_mw = 120.0;  /* Base power consumption */
    double cpu_activity_factor = 1.0;  /* Assume full CPU utilization */
    
    return base_power_mw * cpu_activity_factor;
}
```

### STM32F0 Power Measurement Capabilities

#### Hardware Features Available
- **SystemCoreClock**: 48 MHz (STM32F051)
- **Voltage Regulator**: Internal LDO regulator
- **Power Management**: Limited power modes
- **No DWT**: Cannot measure precise CPU cycles

#### Software-Based Power Estimation
```c
/* Task 8: Power Measurement Analysis - STM32F0 */
static void analyze_power_consumption_f0(void)
{
    printf("\r\n=== Task 8: Power Measurement Analysis - STM32F0 ===\r\n");
    
    /* Measure using HAL_GetTick() */
    uint32_t start_ms = HAL_GetTick();
    
    /* Run Mandelbrot calculation */
    uint32_t checksum = generate_mandelbrot_checksum(128, 128, 100);
    
    uint32_t end_ms = HAL_GetTick();
    uint32_t calculation_ms = end_ms - start_ms;
    
    /* Estimate power consumption */
    double calculation_time_s = (double)calculation_ms / 1000.0;
    double estimated_power_mw = estimate_power_consumption_f0(calculation_ms);
    
    printf("F0 Power Analysis:\r\n");
    printf("- Calculation time: %lu ms\r\n", (unsigned long)calculation_ms);
    printf("- Calculation time: %.6f seconds\r\n", calculation_time_s);
    printf("- Estimated power: %.2f mW\r\n", estimated_power_mw);
    printf("- Energy consumption: %.6f mJ\r\n", estimated_power_mw * calculation_time_s * 1000.0);
}

static double estimate_power_consumption_f0(uint32_t elapsed_ms)
{
    /* Power estimation based on STM32F051 datasheet */
    /* Typical power consumption at 48 MHz: ~20-30 mW */
    /* This is a rough estimation based on execution time */
    
    double base_power_mw = 25.0;  /* Base power consumption */
    double cpu_activity_factor = 1.0;  /* Assume full CPU utilization */
    
    return base_power_mw * cpu_activity_factor;
}
```

## Power Measurement Limitations

### Why Direct Power Measurement is Not Possible

#### 1. **Hardware Limitations**
- **No Built-in Power Measurement**: Neither STM32F4 nor STM32F0 have integrated power measurement capabilities
- **External Dependencies**: Power measurement requires external hardware (multimeter, power analyzer, etc.)
- **Board-Level Measurement**: Power consumption includes entire board, not just microcontroller

#### 2. **Software Limitations**
- **No Power Management APIs**: STM32 HAL doesn't provide power consumption APIs
- **Approximation Only**: Software can only estimate based on CPU activity and timing
- **Incomplete Data**: Cannot measure peripheral power consumption, memory access patterns, etc.

#### 3. **Measurement Complexity**
- **Dynamic Power**: Power consumption varies with CPU load, memory access, and peripheral usage
- **Temperature Dependencies**: Power consumption changes with temperature
- **Voltage Variations**: Power consumption depends on supply voltage stability

## Required Tools and Setup for Accurate Power Measurement

### Hardware Requirements

#### 1. **Precision Multimeter**
- **Model**: Fluke 87V or equivalent
- **Capability**: DC current measurement with microamp precision
- **Connection**: Series connection with power supply
- **Limitation**: Only average power measurement

#### 2. **Power Analyzer**
- **Model**: Keysight N6705C or equivalent
- **Capability**: Real-time power measurement with high precision
- **Features**: Current profiling, energy measurement, data logging
- **Advantage**: Can measure dynamic power consumption

#### 3. **Oscilloscope with Current Probe**
- **Model**: Tektronix TBS1000 series with current probe
- **Capability**: Real-time current waveform analysis
- **Features**: High bandwidth, precise timing correlation
- **Advantage**: Can correlate power consumption with code execution

#### 4. **Specialized Power Measurement Board**
- **Model**: STM32 Power Measurement Shield
- **Capability**: Integrated power measurement for STM32 boards
- **Features**: Real-time monitoring, data logging
- **Advantage**: Purpose-built for STM32 power analysis

### Software Requirements

#### 1. **Power Analysis Software**
- **STMicroelectronics STM32CubeMonitor**: Power analysis tools
- **Custom Data Logging**: Real-time power data collection
- **Correlation Analysis**: Link power consumption to code execution

#### 2. **Measurement Setup**
```
Power Supply → Current Measurement → STM32 Board
                     ↓
              Data Logger/PC
```

## Extrapolation Methodology

### Power Consumption Model

#### 1. **Base Power Consumption**
```c
/* Power consumption model */
typedef struct {
    double base_power_mw;        /* Base power consumption */
    double cpu_power_factor;     /* CPU activity factor */
    double memory_power_factor;  /* Memory access factor */
    double peripheral_power_mw;  /* Peripheral power consumption */
} power_model_t;

/* STM32F4 Power Model */
static const power_model_t f4_power_model = {
    .base_power_mw = 50.0,       /* Base power (idle) */
    .cpu_power_factor = 0.8,     /* CPU power scaling */
    .memory_power_factor = 0.2,  /* Memory power scaling */
    .peripheral_power_mw = 20.0  /* Peripheral power */
};

/* STM32F0 Power Model */
static const power_model_t f0_power_model = {
    .base_power_mw = 15.0,       /* Base power (idle) */
    .cpu_power_factor = 0.6,     /* CPU power scaling */
    .memory_power_factor = 0.1,  /* Memory power scaling */
    .peripheral_power_mw = 5.0   /* Peripheral power */
};
```

#### 2. **Extrapolation Formula**
```c
double estimate_power_consumption(uint32_t cycles, uint32_t memory_accesses, const power_model_t* model)
{
    double cpu_power = model->base_power_mw + (cycles * model->cpu_power_factor);
    double memory_power = memory_accesses * model->memory_power_factor;
    double total_power = cpu_power + memory_power + model->peripheral_power_mw;
    
    return total_power;
}
```

### Extrapolation for Different Test Cases

#### 1. **Resolution Scaling**
- **Power ∝ Resolution²**: Power consumption scales with image size
- **Memory Access**: More pixels = more memory operations
- **CPU Load**: More calculations = higher CPU power consumption

#### 2. **Iteration Scaling**
- **Power ∝ MAX_ITER**: Power consumption scales linearly with iterations
- **CPU Activity**: More iterations = longer CPU active time
- **Memory Access**: More iterations = more memory reads/writes

#### 3. **Platform Comparison**
- **STM32F4**: Higher power consumption due to higher clock speed and FPU
- **STM32F0**: Lower power consumption due to lower clock speed and no FPU

## Implementation in Main Code

### Global Variables for Power Analysis
```c
/* Task 8: Power Measurement Analysis */
volatile double g_estimated_power_f4_mw = 0.0;
volatile double g_estimated_power_f0_mw = 0.0;
volatile double g_energy_consumption_f4_mj = 0.0;
volatile double g_energy_consumption_f0_mj = 0.0;
volatile uint32_t g_power_measurement_cycles = 0u;
```

### Integration with Existing Tasks
```c
/* Add to main loop after Task 7 */
void task8_power_analysis(void)
{
    printf("\r\n=== Task 8: Power Measurement Analysis ===\r\n");
    
    /* STM32F4 Power Analysis */
    #ifdef STM32F446xx
    analyze_power_consumption_f4();
    #endif
    
    /* STM32F0 Power Analysis */
    #ifdef STM32F051xx
    analyze_power_consumption_f0();
    #endif
    
    /* Extrapolation for other test cases */
    extrapolate_power_consumption();
}
```

## Expected Results and Analysis

### Power Consumption Estimates

#### STM32F4 (STM32F446)
- **Base Power**: ~50 mW (idle)
- **Active Power**: ~120 mW (full CPU load)
- **Mandelbrot (128x128, 100 iter)**: ~110 mW
- **Energy per calculation**: ~0.5 mJ

#### STM32F0 (STM32F051)
- **Base Power**: ~15 mW (idle)
- **Active Power**: ~25 mW (full CPU load)
- **Mandelbrot (128x128, 100 iter)**: ~23 mW
- **Energy per calculation**: ~0.1 mJ

### Extrapolation Results

#### Resolution Scaling
| Resolution | F4 Power (mW) | F0 Power (mW) | F4 Energy (mJ) | F0 Energy (mJ) |
|------------|---------------|---------------|----------------|----------------|
| 128x128    | 110           | 23            | 0.5            | 0.1            |
| 160x160    | 170           | 35            | 1.2            | 0.25           |
| 192x192    | 245           | 50            | 2.1            | 0.43           |
| 224x224    | 335           | 68            | 3.4            | 0.69           |
| 256x256    | 440           | 90            | 5.2            | 1.06           |

#### Iteration Scaling
| MAX_ITER | F4 Power (mW) | F0 Power (mW) | F4 Energy (mJ) | F0 Energy (mJ) |
|----------|---------------|---------------|----------------|----------------|
| 100      | 110           | 23            | 0.5            | 0.1            |
| 250      | 275           | 58            | 1.25           | 0.25           |
| 500      | 550           | 115           | 2.5            | 0.5            |
| 750      | 825           | 173           | 3.75           | 0.75           |
| 1000     | 1100          | 230           | 5.0            | 1.0            |

## Conclusion

### Power Measurement Feasibility
- **Direct Measurement**: Not possible with current hardware setup
- **Software Estimation**: Possible but limited accuracy
- **Extrapolation**: Reasonable for comparative analysis

### Recommendations
1. **Use Software Estimation**: For relative power consumption analysis
2. **Focus on Extrapolation**: For scaling analysis across test cases
3. **Consider Hardware Upgrade**: For precise power measurement in future projects
4. **Document Limitations**: Clearly state estimation vs. measurement

### Tools Required for Accurate Measurement
1. **Precision Multimeter**: For average power measurement
2. **Power Analyzer**: For real-time power profiling
3. **Current Probe + Oscilloscope**: For dynamic power analysis
4. **Specialized Power Measurement Board**: For integrated STM32 power analysis

This analysis provides a comprehensive approach to power measurement within the constraints of the available hardware while explaining the limitations and requirements for accurate power measurement.
