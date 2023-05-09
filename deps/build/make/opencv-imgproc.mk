
include platforms/detect-host.mk

NAME       = opencv-imgproc
INCDIR     = ../../include
SRCDIR     = ../../src/$(NAME)
OBJDIR    := $(OBJDIR)/$(NAME)
DEFINES   := $(DEFINES)\
             -DHAVE_CONFIG_H \
             -D__OPENCV_BUILD \
             -D_MT \
             -Wno-return-type-c-linkage \
             -Wno-invalid-noreturn
INCLUDES  := $(INCLUDES) \
            -I$(INCDIR) \
            -I$(SRCDIR) \
            -I$(INCDIR)/opencv2
CXXFLAGS  := $(CXXFLAGS) -std=c++17
LINKFLAGS := $(LINKFLAGS)
LINKPATH  := $(LINKPATH)
LINKLIBS  := $(LINKLIBS)
OBJS       = accum.o \
             accum.dispatch.o \
             approx.o \
             bilateral_filter.dispatch.o \
             blend.o \
             box_filter.dispatch.o \
             canny.o \
             clahe.o \
             color.o \
             color_hsv.dispatch.o \
             color_lab.o \
             color_rgb.dispatch.o \
             color_yuv.dispatch.o \
             colormap.o \
             connectedcomponents.o \
             contours.o \
             convhull.o \
             corner.o \
             cornersubpix.o \
             demosaicing.o \
             deriv.o \
             distransform.o \
             drawing.o \
             emd.o \
             featureselect.o \
             filter.dispatch.o \
             floodfill.o \
             gabor.o \
             generalized_hough.o \
             geometry.o \
             grabcut.o \
             hershey_fonts.o \
             histogram.o \
             hough.o \
             imgwarp.o \
             intersection.o \
             linefit.o \
             lsd.o \
             matchcontours.o \
             min_enclosing_triangle.o \
             moments.o \
             morph.dispatch.o \
             phasecorr.o \
             pyramids.o \
             resize.o \
             rotcalipers.o \
             samplers.o \
             segmentation.o \
             shapedescr.o \
             smooth.dispatch.o \
             spatialgradient.o \
             subdivision2d.o \
             sumpixels.dispatch.o \
             tables.o \
             templmatch.o \
             thresh.o \
             undistort.dispatch.o \
             utils.o

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
