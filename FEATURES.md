# 0.1.0 - Debug
- Debugging tools
    - Debug Logger?
        - Default write to std::puts, with standard format 100%
        - Filtering 100%
    - Debug Exception
        - Standard exception 100%
    - Debug StackTrace
        - Implementation of the walker 100%
    - Custom terminate handler (using std::set_terminate)
        - Standard handler 100%
    - Debug Pop-up Message 100%
    - Convert test to google test 100%
# 0.1.1 - Custom Memory
- Buffer     100%
- Allocators
    - Linear 100%
    - Stack  100%
    - Pool   100%
- Allocator wrapper for STL library 100%
- Benchmarks 100%

# 0.1.2 - Thread-Safe Custom Memory
- Thread-Safe Allocators
    - Linear 100%
    - Stack  100%
    - Pool   100%
- Benchmarks 100% (without multithreading)
# 0.1.3 - Job System
- 1. Type Job
    - In pool allocator
- 2. Type Task
    - In pool allocator
    - Can return value (std::future like)
    - Wrapper from function to task (global memory management)
- 3. Type Argument
    - Memory operations, locations are done here (constexpr best)

# 0.1.4 - Job System Logger
- Debug logger using job system
- Test / Benchmark

# 0.1.5 - Device manager
- System information retrieval
    - RAM
    - OS
    - CPU
    - Attached devices
        - Keyboard
        - Mouse
        - Storage
        - Speakers
        - Microphone
        - Networking Card
    - GPU ? (Or only renderer will use display info on it)

# TODO
- Find an hash algorithm with fast compute speed for big data and low probability of collision (none).

