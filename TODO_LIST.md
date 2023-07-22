REMAINDER
https://web.archive.org/web/20180414161550/http://www.chromium.org/developers/coding-style/cpp-dos-and-donts
https://dirtyhandscoding.github.io/category/cc.html
https://dirtyhandscoding.github.io/posts/improvements-to-symbolsort-tool-for-c-code-bloat-analysis.html

### Test Framework ### (0.4)
# pulsar_tester.hpp
[o] test_benchmark
[o] test_unit
[o] test_suite
[o] test_formatter

[o] all_tests

### Thread-Pool ### (0.5)
# iterable.hpp
[o] lock-free ring buffer (samd ring buffer)
[o] mpmc queue
[o] mpsc unbuffered queue

[o] all_tests

# debug.hpp
[o] concurrent logger
[o] use calloc for every string
[o] clean-up

[o] all_tests

# thread_pool.hpp
[o] exception_handling_test (May need some more stability tests)
[o] stability_test
[o] benchmarks

### Container & Memory ### (0.6)
# iterable.hpp
[o] sequence
[o] sequence tests

# memory.hpp
[o] RAII
[o] RAII tests

### UTF-8 Everywhere ### (0.7)
[o] utf8_string
[o] utf8_iterator
[o] utf8_utilities
[o] utf8_format (fmt)
[o] all_tests

### Filesystem ### (0.8)
[o] fs_path
[o] fs_handle
[o] fs_iterator
[x] fs_signals
[x] fs_archive
[x] all_tests

### Memory 2 ### (0.9)
[o] heap_local_allocation rework (heap stack allocator)
    - local can grow.
    - local allocator act as a stack allocator.
[o] heap_cache_allocation rework (ring buffer allocator)
    - cache cannot grow.
    - cache allocator act as a ring buffer.

### Live++ ### (0.10)
[x] host
[x] client
[x] auto-updater
[x] hooks
[x] all_tests

### File handling ### (0.11)
[x] .tga loader
[x] .tga writer
[x] .jpeg loader
[x] .jpeg writer
[x] .wav loader
[x] .wav writer
[x] all_tests

### Networking ### (0.12)
[x] sockets
[x] protocols
[x] all_tests

### Mathematics ### (0.13)
[x] Rewriting matrix as column major order (DOD)
[x] Quaternions
[x] Projections
[x] Transformations
[x] Camera
[x] all_tests

### ECS ### (0.14)
[x] Entity
[x] Components
[x] System
[x] Multithreading
[x] all_tests

### Fixing ### (0.15)
[x] cv2pdb
[x] FMT 10.* update 

