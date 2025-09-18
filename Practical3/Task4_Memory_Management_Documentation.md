# Task 4: Scalability Test - Memory Management Documentation

## Overview
This document explains the memory management approach used to process Full HD (1920×1080) images on STM32F4 and STM32F0 microcontrollers, which have limited internal SRAM.

## Memory Constraints

### STM32F446 (F4 Series)
- **SRAM**: 128 KB
- **Flash**: 512 KB
- **Architecture**: ARM Cortex-M4 with FPU

### STM32F051 (F0 Series)  
- **SRAM**: 8 KB
- **Flash**: 64 KB
- **Architecture**: ARM Cortex-M0

## Memory Requirements Analysis

### Full HD Image Processing
- **Image Size**: 1920 × 1080 = 2,073,600 pixels
- **Memory per pixel**: ~16 bytes (double precision complex numbers)
- **Total memory needed**: ~33.2 MB

### Memory Constraint Comparison
| Platform | Available SRAM | Required Memory | Deficit |
|----------|----------------|-----------------|---------|
| STM32F446 | 128 KB | 33.2 MB | 33.1 MB |
| STM32F051 | 8 KB | 33.2 MB | 33.2 MB |

**Conclusion**: Both platforms have insufficient internal memory to process Full HD images directly.

## Solution: Image Tiling

### Methodology
The image is divided into smaller tiles that fit within the available SRAM, processed sequentially, and the results are combined.

### Tile Size Selection

#### STM32F4 (MAX_TILE_SIZE = 256)
- **Rationale**: 256×256 tiles require ~1 MB of memory for intermediate calculations
- **Memory usage**: Fits comfortably within 128 KB SRAM with room for stack and other variables
- **Performance**: Good balance between memory usage and processing efficiency

#### STM32F0 (MAX_TILE_SIZE = 128)  
- **Rationale**: 128×128 tiles require ~256 KB of memory for intermediate calculations
- **Memory usage**: Conservative approach due to very limited 8 KB SRAM
- **Performance**: More tiles but ensures memory safety

### Tiling Algorithm

```c
// Calculate number of tiles needed
uint32_t tiles_x = (width + MAX_TILE_SIZE - 1) / MAX_TILE_SIZE;
uint32_t tiles_y = (height + MAX_TILE_SIZE - 1) / MAX_TILE_SIZE;

// Process each tile
for (uint32_t tile_y = 0; tile_y < tiles_y; ++tile_y) {
    for (uint32_t tile_x = 0; tile_x < tiles_x; ++tile_x) {
        // Calculate tile boundaries
        uint16_t start_x = tile_x * MAX_TILE_SIZE;
        uint16_t start_y = tile_y * MAX_TILE_SIZE;
        uint16_t end_x = min(start_x + MAX_TILE_SIZE, width);
        uint16_t end_y = min(start_y + MAX_TILE_SIZE, height);
        
        // Process tile with global coordinates
        process_tile(start_x, start_y, end_x, end_y);
    }
}
```

### Why Tiling Was Necessary

1. **Memory Limitation**: Internal SRAM cannot accommodate full image data
2. **Processing Continuity**: Mandelbrot calculation requires global coordinate mapping
3. **Result Integrity**: Each pixel's iteration count contributes to the final checksum
4. **Scalability**: Approach works for any image size beyond memory constraints

### Implementation Details

#### Coordinate Mapping
- Each tile maintains global coordinate system
- `global_x = start_x + local_x`
- `global_y = start_y + local_y`
- Complex plane mapping: `x0 = (global_x / width) * 3.5 - 2.5`

#### Memory Management
- No intermediate storage of pixel data
- Direct calculation and accumulation of checksum
- Minimal memory footprint per tile

#### Performance Considerations
- **Overhead**: Tile boundary calculations add minimal overhead
- **Cache Efficiency**: Smaller tiles improve cache utilization
- **Processing Time**: Linear increase with number of tiles

### Live Expressions for Monitoring

#### Tiling Information
- `g_num_tiles_x`: Number of tiles in X direction
- `g_num_tiles_y`: Number of tiles in Y direction  
- `g_total_tiles`: Total number of tiles
- `g_current_tile`: Current tile being processed (1-indexed)

#### Performance Metrics
- `checksum[0..7]`: Final checksums for each resolution
- `execution_time_ms[0..7]`: Execution times
- `cpu_cycles[0..7]`: CPU cycle counts
- `throughput_pps[0..7]`: Throughput in pixels per second

### Test Resolutions

| Index | Width | Height | Tiles (F4) | Tiles (F0) |
|-------|-------|--------|------------|------------|
| 0 | 128 | 128 | 1×1 | 1×1 |
| 1 | 256 | 256 | 1×1 | 2×2 |
| 2 | 512 | 512 | 2×2 | 4×4 |
| 3 | 640 | 480 | 3×2 | 5×4 |
| 4 | 800 | 600 | 4×3 | 7×5 |
| 5 | 1024 | 768 | 4×3 | 8×6 |
| 6 | 1280 | 720 | 5×3 | 10×6 |
| 7 | 1920 | 1080 | 8×5 | 15×9 |

### Benefits of This Approach

1. **Memory Safety**: Prevents stack overflow and memory corruption
2. **Scalability**: Can handle images larger than available memory
3. **Flexibility**: Tile size can be adjusted based on available memory
4. **Monitoring**: Real-time progress tracking through Live Expressions
5. **Reliability**: Robust handling of edge cases and boundary conditions

### Limitations

1. **Processing Time**: Sequential tile processing increases total execution time
2. **Memory Fragmentation**: Multiple small allocations may fragment memory
3. **Cache Misses**: Frequent tile switching may reduce cache efficiency

## Conclusion

The tiling approach successfully enables Full HD image processing on memory-constrained microcontrollers by breaking the problem into manageable chunks. This method demonstrates how algorithmic solutions can overcome hardware limitations while maintaining result accuracy and providing real-time monitoring capabilities.
