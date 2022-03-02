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
    - Convert test to google test

# 0.1.1 - Utilities

#
# Main problem
# 1. !LOCK-FREE / !WAIT-FREE
#    -> std::mutex
# 2. !LOCK-FREE / WAIT-FREE
#    -> std::atomic<_Ty>
# 3. LOCK-FREE / WAIT-FREE
#    -> std::atomic_flag 
#    -> https://github.com/khizmax/libcds
#    -> https://superchargedcomputing.com/2018/03/25/atomics-and-lockfree-data-structures-in-c/
#    -> https://www.codeleading.com/article/25062250006/
#

- LIFO + FIFO structures
    - Thread-safe
    - Standard
- Linked-Lists
    - Standard
    - Cyclic
    - Packed
    - Atomic ?
- Trees
    - Binary
    - Red-black
- Tests and benchmarking

# 0.1.2 - Custom Memory
- Memory buffers (atomic & standard)
- Memory provider (atomic & standard)
- Memory custom allocators (atomic & standard)
    - Linear
    - Stack
    - Pool
    - Freelist
- Memory utility functions
- Memory tests and benchmarking
- Operator new / delete overloading with allocators

# 0.1.3 - Job System
- Job system
    - Job structure
    - Task structure
        - 2 insertions types : first / last
        - Handle return values
        - Custom memory management of stored data
            - Pool allocator for tasks and jobs
            - Freelist allocator for unique (sizeable) data
    - Caching system / growing allocation (must be realy fast)
    
- Job system tests and benchmarking

# 0.1.4 - Multithreaded job system
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
        - Internet
    - GPU ? (Or only renderer will use )

# 0.1.6 More utility functions
- Mathematics
    - Matrix
    - Vector
    - Quaternion
    - Some useful mathematics functions
- Color
    - HEXA <=> RGBA
    - Light color <=> RGBA
- Sort functions
- Search functions