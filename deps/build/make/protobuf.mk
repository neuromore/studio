
include platforms/detect-host.mk
include platforms/$(DETECTED_OS)-$(DETECTED_ARCH)-$(TARGET_OS)-$(TARGET_ARCH).mk

NAME       = protobuf
INCDIR     = ../../include/
SRCDIR     = ../../src/$(NAME)
OBJDIR    := $(OBJDIR)/$(NAME)
DEFINES   := $(DEFINES) \
             -DHAVE_ZLIB \
             -DCHROMIUM_ZLIB_NO_CHROMECONF \
             -Wno-invalid-noreturn
INCLUDES  := $(INCLUDES) -I$(INCDIR) -I$(INCDIR)/$(NAME) -I$(INCDIR)/zlib
CFLAGS    := $(CFLAGS)
LINKFLAGS := $(LINKFLAGS)
LINKPATH  := $(LINKPATH)
LINKLIBS  := $(LINKLIBS)
OBJS       = io/coded_stream.o \
             io/gzip_stream.o \
             io/printer.o \
             io/strtod.o \
             io/tokenizer.o \
             io/zero_copy_stream.o \
             io/zero_copy_stream_impl.o \
             io/zero_copy_stream_impl_lite.o \
             stubs/bytestream.o \
             stubs/common.o \
             stubs/int128.o \
             stubs/status.o \
             stubs/statusor.o \
             stubs/stringpiece.o \
             stubs/stringprintf.o \
             stubs/structurally_valid.o \
             stubs/strutil.o \
             stubs/substitute.o \
             stubs/time.o \
             util/internal/datapiece.o \
             util/internal/default_value_objectwriter.o \
             util/internal/error_listener.o \
             util/internal/field_mask_utility.o \
             util/internal/json_escaping.o \
             util/internal/json_objectwriter.o \
             util/internal/json_stream_parser.o \
             util/internal/object_writer.o \
             util/internal/proto_writer.o \
             util/internal/protostream_objectsource.o \
             util/internal/protostream_objectwriter.o \
             util/internal/type_info.o \
             util/internal/utility.o \
             util/delimited_message_util.o \
             util/field_comparator.o \
             util/field_mask_util.o \
             util/json_util.o \
             util/message_differencer.o \
             util/time_util.o \
             util/type_resolver_util.o \
			 any.o \
             any.pb.o \
             any_lite.o \
             api.pb.o \
             arena.o \
             arenastring.o \
             arenaz_sampler.o \
             descriptor.o \
             descriptor.pb.o \
             descriptor_database.o \
             duration.pb.o \
             dynamic_message.o \
             empty.pb.o \
             extension_set.o \
             extension_set_heavy.o \
             field_mask.pb.o \
             generated_enum_util.o \
             generated_message_bases.o \
             generated_message_reflection.o \
             generated_message_tctable_full.o \
             generated_message_tctable_lite.o \
             generated_message_util.o \
             implicit_weak_message.o \
             inlined_string_field.o \
             map.o \
             map_field.o \
             message.o \
             message_lite.o \
             parse_context.o \
             reflection_ops.o \
             repeated_field.o \
             repeated_ptr_field.o \
             service.o \
             source_context.pb.o \
             struct.pb.o \
             text_format.o \
             timestamp.pb.o \
             type.pb.o \
             unknown_field_set.o \
             wire_format.o \
             wire_format_lite.o \
             wrappers.pb.o

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
CFLAGS    := $(CFLAGS)
OBJS      := $(OBJS) io/io_win32.o
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
	$(CC) $(CPUFLAGS) $(DEFINES) $(INCLUDES) $(CFLAGS) -c $(@:$(OBJDIR)%.o=$(SRCDIR)%.cc) -o $@

.DEFAULT_GOAL := build

build: $(OBJS)
	@echo [AR]  $(LIBDIR)/$(NAME)$(SUFFIX)$(EXTLIB)
	$(AR) $(ARFLAGS) $(LIBDIR)/$(NAME)$(SUFFIX)$(EXTLIB) $(OBJS)

clean:
	$(call deletefiles,$(OBJDIR),*.o)
	$(call deletefiles,$(LIBDIR),$(NAME)$(SUFFIX)$(EXTLIB))
