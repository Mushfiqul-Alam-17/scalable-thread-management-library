# Scalable Thread Management Library - Project Report

## Project Overview

This project implements a high-performance, scalable thread pool library in C++ as part of the Operating Systems course (CSE316) at Lovely Professional University. The library provides efficient thread management for concurrent task execution.

## Objectives

1. Implement a scalable thread pool with dynamic sizing
2. Provide thread-safe task submission and execution
3. Monitor thread pool performance and queue metrics
4. Generate performance logs and visualizations
5. Demonstrate efficient concurrent task processing

## Implementation Details

### Core Components

#### ThreadPool Class
- **Worker Thread Management**: Maintains a pool of worker threads that process tasks from a queue
- **Task Queue**: Thread-safe deque for storing pending tasks
- **Synchronization**: Uses mutex and condition variables for thread coordination
- **Atomic Operations**: Tracks active workers using atomic counters

#### Key Features
1. **Task Submission**: Uses `std::packaged_task` and `std::future` for result retrieval
2. **Dynamic Sizing**: Ability to increase thread pool size at runtime
3. **Graceful Shutdown**: Option to wait for all tasks to complete before termination
4. **Performance Monitoring**: Tracks active workers and queued tasks

### Synchronization Mechanisms

- **Mutex (`tasksMutex`)**: Protects the task queue from concurrent access
- **Condition Variable (`cv`)**: Notifies worker threads when tasks are available
- **Atomic Variables**: `stopping` and `activeWorkers` for lock-free operations

### Performance Metrics

- **Tasks Executed**: 20,000
- **CPU Iterations per Task**: 50,000
- **Total Execution Time**: 0.789321 seconds
- **Peak Queue Size**: 17,582 tasks
- **Peak Active Threads**: 8
- **Throughput**: ~25,338 tasks/second

## Testing and Results

### Test Configuration
- Thread Count: 8
- Task Count: 20,000
- Work Iterations: 50,000 per task

### Performance Analysis

The thread pool demonstrates efficient task distribution:
- Queue size grows initially as tasks are submitted faster than they can be processed
- Active threads reach maximum capacity (8) quickly
- Queue gradually decreases as tasks complete
- High throughput achieved with minimal overhead

### CSV Logging

Performance data is logged to `tests/stats.csv` with columns:
- `submitted`: Number of tasks submitted
- `queued`: Number of tasks waiting in queue
- `active`: Number of threads currently executing tasks

## Visualization

Performance graphs are generated using Python scripts in Google Colab:
- Queue size over time (`queue.png`)
- Active threads over time (`active_threads.png`)

## Conclusion

The scalable thread management library successfully demonstrates:
- Efficient concurrent task processing
- Thread-safe operations
- Performance monitoring capabilities
- Scalable architecture for high-throughput applications

## Future Enhancements

1. Priority-based task scheduling
2. Task cancellation support
3. Thread pool size reduction capability
4. More detailed performance metrics
5. Support for task dependencies

## Authors

- Mushfiq Alam (RK24PMB40)
- Indulekha S (RK24PMB41)
- Satyam Kumar (RK24PMB39)

## License

Educational use only.
