
include platforms/detect-host.mk

NAME       = opencv-core
INCDIR     = ../../include
SRCDIR     = ../../src/$(NAME)
OBJDIR    := $(OBJDIR)/$(NAME)
DEFINES   := $(DEFINES)\
             -DCHROMIUM_ZLIB_NO_CHROMECONF \
             -DHAVE_CONFIG_H \
             -D__OPENCV_BUILD \
             -D_MT \
             -Wno-comment \
             -Wno-return-type-c-linkage \
             -Wno-invalid-noreturn
INCLUDES  := $(INCLUDES) \
            -I$(INCDIR) \
            -I$(SRCDIR) \
            -I$(INCDIR)/opencv2 \
            -I$(INCDIR)/zlib
CXXFLAGS  := $(CXXFLAGS) -std=c++17
LINKFLAGS := $(LINKFLAGS)
LINKPATH  := $(LINKPATH)
LINKLIBS  := $(LINKLIBS)
OBJS       = utils/datafile.o \
             utils/filesystem.o \
             utils/samples.o \
             algorithm.o \
             alloc.o \
             arithm.o \
             arithm.dispatch.o \
             array.o \
             async.o \
             batch_distance.o \
             bindings_utils.o \
             buffer_area.o \
             channels.o \
             check.o \
             command_line_parser.o \
             conjugate_gradient.o \
             convert.dispatch.o \
             convert_c.o \
             convert_scale.dispatch.o \
             copy.o \
             count_non_zero.dispatch.o \
             cuda_gpu_mat.o \
             cuda_host_mem.o \
             cuda_info.o \
             cuda_stream.o \
             datastructs.o \
             downhill_simplex.o \
             dxt.o \
             gl_core_3_1.o \
             glob.o \
             hal_internal.o \
             kmeans.o \
             lapack.o \
             lda.o \
             logger.o \
             lpsolver.o \
             lut.o \
             mathfuncs.o \
             mathfuncs_core.dispatch.o \
             matmul.dispatch.o \
             matrix.o \
             matrix_c.o \
             matrix_decomp.o \
             matrix_expressions.o \
             matrix_iterator.o \
             matrix_operations.o \
             matrix_sparse.o \
             matrix_wrap.o \
             mean.dispatch.o \
             merge.dispatch.o \
             minmax.o \
             norm.o \
             ocl.o \
             opengl.o \
             out.o \
             ovx.o \
             parallel.o \
             parallel_impl.o \
             pca.o \
             persistence.o \
             persistence_base64.o \
             persistence_c.o \
             persistence_cpp.o \
             persistence_json.o \
             persistence_types.o \
             persistence_xml.o \
             persistence_yml.o \
             rand.o \
             softfloat.o \
             split.dispatch.o \
             stat.dispatch.o \
             stat_c.o \
             stl.o \
             sum.dispatch.o \
             system.o \
             tables.o \
             trace.o \
             types.o \
             umatrix.o \
             va_intel.o

ifeq ($(TARGET_ARCH),x86)
DEFINES   := $(DEFINES)
endif

ifeq ($(TARGET_ARCH),x64)
DEFINES   := $(DEFINES)
endif

ifeq ($(TARGET_ARCH),arm)
DEFINES   := $(DEFINES)
endif

ifeq ($(TARGET_ARCH),arm64)
DEFINES   := $(DEFINES)
endif

ifeq ($(TARGET_OS),win)
DEFINES   := $(DEFINES) -D_CRT_SECURE_NO_WARNINGS
CXXFLAGS  := $(CXXFLAGS)
OBJS      := $(OBJS) directx.o
ifeq ($(TARGET_ARCH),x86)
DEFINES   := $(DEFINES)
endif
ifeq ($(TARGET_ARCH),x64)
DEFINES   := $(DEFINES) 
endif
ifeq ($(TARGET_ARCH),arm)
DEFINES   := $(DEFINES)
endif
ifeq ($(TARGET_ARCH),arm64)
DEFINES   := $(DEFINES)
endif
endif

ifeq ($(TARGET_OS),osx)
DEFINES   := $(DEFINES)
CXXFLAGS  := $(CXXFLAGS)
ifeq ($(TARGET_ARCH),x86)
DEFINES   := $(DEFINES)
endif
ifeq ($(TARGET_ARCH),x64)
DEFINES   := $(DEFINES)
endif
ifeq ($(TARGET_ARCH),arm)
DEFINES   := $(DEFINES)
endif
ifeq ($(TARGET_ARCH),arm64)
DEFINES   := $(DEFINES)
endif
endif

ifeq ($(TARGET_OS),linux)
DEFINES   := $(DEFINES)
CXXFLAGS  := $(CXXFLAGS)
ifeq ($(TARGET_ARCH),x86)
DEFINES   := $(DEFINES)
endif
ifeq ($(TARGET_ARCH),x64)
DEFINES   := $(DEFINES)
endif
ifeq ($(TARGET_ARCH),arm)
DEFINES   := $(DEFINES)
endif
ifeq ($(TARGET_ARCH),arm64)
DEFINES   := $(DEFINES)
endif
endif

ifeq ($(TARGET_OS),android)
DEFINES   := $(DEFINES)
CXXFLAGS  := $(CXXFLAGS)
ifeq ($(TARGET_ARCH),x86)
DEFINES   := $(DEFINES)
endif
ifeq ($(TARGET_ARCH),x64)
DEFINES   := $(DEFINES)
endif
ifeq ($(TARGET_ARCH),arm)
DEFINES   := $(DEFINES)
endif
ifeq ($(TARGET_ARCH),arm64)
DEFINES   := $(DEFINES)
endif
endif

OBJS := $(patsubst %,$(OBJDIR)/%,$(OBJS))

$(OBJDIR)/%.o:
	@echo [CXX] $@
	$(CXX) $(CPUFLAGS) $(DEFINES) $(INCLUDES) $(CXXFLAGS) -c $(@:$(OBJDIR)%.o=$(SRCDIR)%.cpp) -o $@

.DEFAULT_GOAL := build

build: $(OBJS)
	@echo [AR]  $(LIBDIR)/$(NAME)$(SUFFIX)$(EXTLIB)
	$(AR) $(ARFLAGS) $(LIBDIR)/$(NAME)$(SUFFIX)$(EXTLIB) $(OBJS)

clean:
	$(call deletefiles,$(OBJDIR),*.o)
	$(call deletefiles,$(LIBDIR),$(NAME)$(SUFFIX)$(EXTLIB))
