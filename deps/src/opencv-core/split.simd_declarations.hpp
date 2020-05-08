#define CV_CPU_SIMD_FILENAME "split.simd.hpp"
#define CV_CPU_DISPATCH_MODE SSE2
#include "opencv2/core/private/cv_cpu_include_simd_declarations.hpp"

#define CV_CPU_DISPATCH_MODES_ALL SSE2, BASELINE

#undef CV_CPU_SIMD_FILENAME
