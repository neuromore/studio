
include platforms/detect-host.mk
include platforms/$(DETECTED_OS)-$(DETECTED_ARCH)-$(TARGET_OS)-$(TARGET_ARCH).mk

NAME       = libssl
INCDIR     = ../../include
SRCDIR     = ../../src/openssl
OBJDIR    := $(OBJDIR)/$(NAME)
DEFINES   := $(DEFINES) \
             -DHAVE_CONFIG_H \
             -DOPENSSL_STATIC \
             -DOPENSSL_NO_DYNAMIC_ENGINE \
             -DOPENSSL_THREADS \
             -DOPENSSL_PIC \
             -DL_ENDIAN
INCLUDES  := $(INCLUDES) \
             -I$(INCDIR) \
             -I$(SRCDIR) \
             -I$(INCDIR)/openssl \
             -I$(SRCDIR)/modes
CFLAGS    := $(CFLAGS)
LINKFLAGS := $(LINKFLAGS)
LINKPATH  := $(LINKPATH)
LINKLIBS  := $(LINKLIBS)
OBJS       = engines/e_capi.o \
             engines/e_padlock.o \
             ssl/record/dtls1_bitmap.o \
             ssl/record/rec_layer_d1.o \
             ssl/record/rec_layer_s3.o \
             ssl/record/ssl3_buffer.o \
             ssl/record/ssl3_record.o \
             ssl/statem/statem.o \
             ssl/statem/statem_clnt.o \
             ssl/statem/statem_dtls.o \
             ssl/statem/statem_lib.o \
             ssl/statem/statem_srvr.o \
             ssl/bio_ssl.o \
             ssl/d1_lib.o \
             ssl/d1_msg.o \
             ssl/d1_srtp.o \
             ssl/methods.o \
             ssl/pqueue.o \
             ssl/s3_cbc.o \
             ssl/s3_enc.o \
             ssl/s3_lib.o \
             ssl/s3_msg.o \
             ssl/ssl_asn1.o \
             ssl/ssl_cert.o \
             ssl/ssl_ciph.o \
             ssl/ssl_conf.o \
             ssl/ssl_err.o \
             ssl/ssl_init.o \
             ssl/ssl_lib.o \
             ssl/ssl_mcnf.o \
             ssl/ssl_rsa.o \
             ssl/ssl_sess.o \
             ssl/ssl_stat.o \
             ssl/ssl_txt.o \
             ssl/ssl_utst.o \
             ssl/t1_enc.o \
             ssl/t1_ext.o \
             ssl/t1_lib.o \
             ssl/t1_reneg.o \
             ssl/t1_trce.o \
             ssl/tls_srp.o

ifeq ($(TARGET_ARCH),x86)
DEFINES   := $(DEFINES) -DOPENSSL_IA32_SSE2
endif

ifeq ($(TARGET_ARCH),x64)
DEFINES   := $(DEFINES) -DOPENSSL_IA32_SSE2
endif

ifeq ($(TARGET_ARCH),arm)
DEFINES   := $(DEFINES)
endif

ifeq ($(TARGET_ARCH),arm64)
DEFINES   := $(DEFINES)
endif

ifeq ($(TARGET_OS),win)
DEFINES   := $(DEFINES) \
             -DDSO_WIN32 \
             -DWIN32_LEAN_AND_MEAN \
             -DOPENSSL_USE_APPLINK \
             -DOPENSSLDIR=\".\" \
             -DENGINESDIR=\".\" 
CFLAGS    := $(CFLAGS)
ifeq ($(TARGET_ARCH),x86)
DEFINES   := $(DEFINES) -DOPENSSL_SYS_WIN32
endif
ifeq ($(TARGET_ARCH),x64)
DEFINES   := $(DEFINES) -DOPENSSL_SYS_WIN64
endif
ifeq ($(TARGET_ARCH),arm)
DEFINES   := $(DEFINES)
endif
ifeq ($(TARGET_ARCH),arm64)
DEFINES   := $(DEFINES)
endif
endif

ifeq ($(TARGET_OS),osx)
DEFINES   := $(DEFINES) -DOPENSSLDIR=\"/etc/ssl\" -DENGINESDIR=\".\" 
CFLAGS    := $(CFLAGS)
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
DEFINES   := $(DEFINES) -DOPENSSLDIR=\"/etc/ssl\" -DENGINESDIR=\".\" 
CFLAGS    := $(CFLAGS)
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
CFLAGS    := $(CFLAGS)
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
	@echo [CC]  $@
	$(CC) $(CPUFLAGS) $(DEFINES) $(INCLUDES) $(CFLAGS) -c $(@:$(OBJDIR)%.o=$(SRCDIR)%.c) -o $@

.DEFAULT_GOAL := build

build: $(OBJS)
	@echo [AR]  $(LIBDIR)/$(NAME)$(SUFFIX)$(EXTLIB)
	$(AR) $(ARFLAGS) $(LIBDIR)/$(NAME)$(SUFFIX)$(EXTLIB) $(OBJS)

clean:
	$(call deletefiles,$(OBJDIR),*.o)
	$(call deletefiles,$(LIBDIR),$(NAME)$(SUFFIX)$(EXTLIB))
