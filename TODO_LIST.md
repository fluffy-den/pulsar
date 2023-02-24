REMAINDER
https://web.archive.org/web/20180414161550/http://www.chromium.org/developers/coding-style/cpp-dos-and-donts
https://dirtyhandscoding.github.io/category/cc.html
https://dirtyhandscoding.github.io/posts/improvements-to-symbolsort-tool-for-c-code-bloat-analysis.html

### Test Framework ### (0.4)
# container.hpp
[o] array
[o] forward list
[o] tests
[o] span
[o] forward sequence

# utility.hpp
[o] concepts
[o] inserter
[o] mover
[o] replacer

# memory.hpp
[o] singly data structure
[o] doubly data structure
[o] reallocate
[o] memory_array -> mi_function

# debug.hpp
[o] exception
[o] error code
[o] error category
[o] logger (not thread-safe)

# encoding.hpp
[o] utf8
[o] ascii
[o] wide
[o] char trait transcoding
[o] char trait format
[o] char trait string
[o] char trait span
[o] char trait iterator

# pulsar_tester.hpp
[o] test_benchmark
[o] test_unit
[o] test_suite
[o] test_formatter

[o] all_tests


### Thread-Pool ### (0.5)
# concurrency.hpp
[o] inline task
[o] shared task
[o] parallel_for
[o] inline job
[o] shared job
[o] future

# debug.hpp
[o] concurrent logger

[o] all_tests


### Filesystem ### (0.6)
# filesystem.hpp
[o] fshandle
[o] fsiterator
[o] fspack
[o] fspath
[o] fssignal
[o] fsformat

[o] all_tests


### Live++ ### (0.7)

# debug.hpp
[o] stack walker
[o] dump file

# live++.hpp
// https://github.com/RuntimeCompiledCPlusPlus/RuntimeCompiledCPlusPlus
[o] runtime compiled C++

[o] all_tests


### Networking ### (0.8)
# network.hpp
[o] asio

[o] all_tests


### Utility ### (1.0)
# encryption.hpp
# Some algorithms that encrypt (fast + reliable)

# mathematics.hpp
# Some linear algebra algorithms

# utility.hpp
# Some more utilities

# container.hpp
# Some more containers