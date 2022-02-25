# Planned features
# 0.1.0 Debug
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
# 0.1.2 
- Update Cmake
    - Compiler flags (for MSVC and GCC / Clang)
    - Options implementation
    - Cleaning
# 0.2.0 Custom Allocators
- Memory custom allocators (atomic & standard)
    - Linear
    - Stack
    - Pool
    - Slab
    - Freelist
- Memory utility functions
- Memory tests and benchmarking
- Operator new / delete overloading with allocators
# 0.3.0 Job System
- Job system
- Job system tests and benchmarking

# 0.3.1
- Deubg logger using job system
# 0.4.0
- System information retrieval
    - RAM
    - OS
    - CPU
    - Attached devices
        - Keyboard
        - Mouse
        - Storage devices (can be usefull) (To think about)
        - Phones
        - Microphones
# 0.5.0 More utility functions
- Utilities functions (HEXA <-> RGBA)
- Mathematics vector, matrix (glm?)

## TO THINK ABOUT ##
- Custom containers with atomic (vector, list, deck, queue, ...)
- 