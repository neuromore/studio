///////////////////////////////////////////////////////////////////////////////////
// X86/X64
///////////////////////////////////////////////////////////////////////////////////
#if defined(__i386__) || defined(__x86_64__) || defined(_M_IX86) || defined(_M_X64)

#define CV_CPU_COMPILE_SSE 1
#define CV_CPU_BASELINE_COMPILE_SSE 1

#define CV_CPU_COMPILE_SSE2 1
#define CV_CPU_BASELINE_COMPILE_SSE2 1

//#define CV_CPU_COMPILE_SSE3 1
//#define CV_CPU_BASELINE_COMPILE_SSE3 1

#define CV_CPU_BASELINE_FEATURES 0 \
    , CV_CPU_SSE \
    , CV_CPU_SSE2 \

/*#define CV_CPU_BASELINE_FEATURES 0 \
    , CV_CPU_SSE \
    , CV_CPU_SSE2 \
    , CV_CPU_SSE3 \*/

/*#define CV_CPU_DISPATCH_COMPILE_SSE4_1 1
#define CV_CPU_DISPATCH_COMPILE_SSE4_2 1
#define CV_CPU_DISPATCH_COMPILE_FP16 1
#define CV_CPU_DISPATCH_COMPILE_AVX 1
#define CV_CPU_DISPATCH_COMPILE_AVX2 1
#define CV_CPU_DISPATCH_COMPILE_AVX512_SKX 1*/

#define CV_CPU_DISPATCH_FEATURES 0 \

/*#define CV_CPU_DISPATCH_FEATURES 0 \
    , CV_CPU_SSE4_1 \
    , CV_CPU_SSE4_2 \
    , CV_CPU_FP16 \
    , CV_CPU_AVX \
    , CV_CPU_AVX2 \
    , CV_CPU_AVX512_SKX \*/

///////////////////////////////////////////////////////////////////////////////////
// ARM
///////////////////////////////////////////////////////////////////////////////////
#elif defined(__arm__) || defined(__aarch64__) || defined(_M_ARM)

#define CV_CPU_BASELINE_FEATURES 0

#define CV_CPU_DISPATCH_FEATURES 0

///////////////////////////////////////////////////////////////////////////////////
// UNKNOWN
///////////////////////////////////////////////////////////////////////////////////
#else

#define CV_CPU_BASELINE_FEATURES 0

#define CV_CPU_DISPATCH_FEATURES 0

#endif
