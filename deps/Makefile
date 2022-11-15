
include ./build/make/platforms/detect-host.mk

# Use Detected Platform as Target by default

ifeq ($(TARGET_OS),)
TARGET_OS = $(DETECTED_OS)
endif

ifeq ($(TARGET_ARCH),)
TARGET_ARCH = $(DETECTED_ARCH)
endif

##################################################################################
# ALL PLATFORMS
##################################################################################

brainflow:
	+@make -s -C ./build/make/ -f brainflow.mk

brainflow-clean:
	+@make -s -C ./build/make/ -f brainflow.mk clean

brainflow-boardcontroller:
	+@make -s -C ./build/make/ -f brainflow-boardcontroller.mk

brainflow-boardcontroller-clean:
	+@make -s -C ./build/make/ -f brainflow-boardcontroller.mk clean

brainflow-datahandler:
	+@make -s -C ./build/make/ -f brainflow-datahandler.mk

brainflow-datahandler-clean:
	+@make -s -C ./build/make/ -f brainflow-datahandler.mk clean

brainflow-ganglion-bglib: brainflow
	+@make -s -C ./build/make/ -f brainflow-ganglion-bglib.mk

brainflow-ganglion-bglib-clean: brainflow-clean
	+@make -s -C ./build/make/ -f brainflow-ganglion-bglib.mk clean

brainflow-ml:
	+@make -s -C ./build/make/ -f brainflow-ml.mk

brainflow-ml-clean:
	+@make -s -C ./build/make/ -f brainflow-ml.mk clean

double-conversion:
	+@make -s -C ./build/make/ -f double-conversion.mk

double-conversion-clean:
	+@make -s -C ./build/make/ -f double-conversion.mk clean

dspfilters:
	+@make -s -C ./build/make/ -f dspfilters.mk

dspfilters-clean:
	+@make -s -C ./build/make/ -f dspfilters.mk clean

edflib:
	+@make -s -C ./build/make/ -f edflib.mk

edflib-clean:
	+@make -s -C ./build/make/ -f edflib.mk clean

freetype:
	+@make -s -C ./build/make/ -f freetype.mk

freetype-clean:
	+@make -s -C ./build/make/ -f freetype.mk clean

harfbuzz:
	+@make -s -C ./build/make/ -f harfbuzz.mk

harfbuzz-clean:
	+@make -s -C ./build/make/ -f harfbuzz.mk clean

harfbuzz-ng:
	+@make -s -C ./build/make/ -f harfbuzz-ng.mk

harfbuzz-ng-clean:
	+@make -s -C ./build/make/ -f harfbuzz-ng.mk clean

kissfft:
	+@make -s -C ./build/make/ -f kissfft.mk

kissfft-clean:
	+@make -s -C ./build/make/ -f kissfft.mk clean

libcrypto:
	+@make -s -C ./build/make/ -f libcrypto.mk

libcrypto-clean:
	+@make -s -C ./build/make/ -f libcrypto.mk clean

libjpeg:
	+@make -s -C ./build/make/ -f libjpeg.mk

libjpeg-clean:
	+@make -s -C ./build/make/ -f libjpeg.mk clean

libpng:
	+@make -s -C ./build/make/ -f libpng.mk

libpng-clean:
	+@make -s -C ./build/make/ -f libpng.mk clean

libssl:
	+@make -s -C ./build/make/ -f libssl.mk

libssl-clean:
	+@make -s -C ./build/make/ -f libssl.mk clean

libsvm:
	+@make -s -C ./build/make/ -f libsvm.mk

libsvm-clean:
	+@make -s -C ./build/make/ -f libsvm.mk clean

minizip:
	+@make -s -C ./build/make/ -f minizip.mk

minizip-clean:
	+@make -s -C ./build/make/ -f minizip.mk clean

opencv-core:
	+@make -s -C ./build/make/ -f opencv-core.mk

opencv-core-clean:
	+@make -s -C ./build/make/ -f opencv-core.mk clean

opencv-imgcodecs:
	+@make -s -C ./build/make/ -f opencv-imgcodecs.mk

opencv-imgcodecs-clean:
	+@make -s -C ./build/make/ -f opencv-imgcodecs.mk clean

opencv-imgproc:
	+@make -s -C ./build/make/ -f opencv-imgproc.mk

opencv-imgproc-clean:
	+@make -s -C ./build/make/ -f opencv-imgproc.mk clean

opencv-videoio:
	+@make -s -C ./build/make/ -f opencv-videoio.mk

opencv-videoio-clean:
	+@make -s -C ./build/make/ -f opencv-videoio.mk clean

oscpack:
	+@make -s -C ./build/make/ -f oscpack.mk

oscpack-clean:
	+@make -s -C ./build/make/ -f oscpack.mk clean

pcre2:
	+@make -s -C ./build/make/ -f pcre2.mk

pcre2-clean:
	+@make -s -C ./build/make/ -f pcre2.mk clean

qt-bluetooth: qt-moc qt-rcc qt-uic
	+@make -s -C ./build/make/ -f qt-bluetooth.mk

qt-bluetooth-clean: qt-moc-clean qt-rcc-clean qt-uic-clean
	+@make -s -C ./build/make/ -f qt-bluetooth.mk clean

qt-bootstrap:
	+@make -s -C ./build/make/ -f qt-bootstrap.mk

qt-bootstrap-clean:
	+@make -s -C ./build/make/ -f qt-bootstrap.mk clean

qt-concurrent: qt-moc qt-rcc qt-uic
	+@make -s -C ./build/make/ -f qt-concurrent.mk

qt-concurrent-clean: qt-moc-clean qt-rcc-clean qt-uic-clean
	+@make -s -C ./build/make/ -f qt-concurrent.mk clean

qt-core: qt-moc qt-rcc qt-uic
	+@make -s -C ./build/make/ -f qt-core.mk

qt-core-clean: qt-moc-clean qt-rcc-clean qt-uic-clean
	+@make -s -C ./build/make/ -f qt-core.mk clean

qt-eventdispatchers: qt-moc qt-rcc qt-uic
	+@make -s -C ./build/make/ -f qt-eventdispatchers.mk

qt-eventdispatchers-clean: qt-moc-clean qt-rcc-clean qt-uic-clean
	+@make -s -C ./build/make/ -f qt-eventdispatchers.mk clean

qt-fontdatabases: qt-moc qt-rcc qt-uic
	+@make -s -C ./build/make/ -f qt-fontdatabases.mk

qt-fontdatabases-clean: qt-moc-clean qt-rcc-clean qt-uic-clean
	+@make -s -C ./build/make/ -f qt-fontdatabases.mk clean

qt-gamepad: qt-moc qt-rcc qt-uic
	+@make -s -C ./build/make/ -f qt-gamepad.mk

qt-gamepad-clean: qt-moc-clean qt-rcc-clean qt-uic-clean
	+@make -s -C ./build/make/ -f qt-gamepad.mk clean

qt-gui: qt-moc qt-rcc qt-uic
	+@make -s -C ./build/make/ -f qt-gui.mk

qt-gui-clean: qt-moc-clean qt-rcc-clean qt-uic-clean
	+@make -s -C ./build/make/ -f qt-gui.mk clean

qt-moc: zlib qt-bootstrap
	+@make -s -C ./build/make/ -f qt-moc.mk

qt-moc-clean: zlib-clean qt-bootstrap-clean
	+@make -s -C ./build/make/ -f qt-moc.mk clean

qt-multimedia: qt-moc qt-rcc qt-uic
	+@make -s -C ./build/make/ -f qt-multimedia.mk

qt-multimedia-clean: qt-moc-clean qt-rcc-clean qt-uic-clean
	+@make -s -C ./build/make/ -f qt-multimedia.mk clean

qt-multimediaplugins: qt-moc qt-rcc qt-uic
	+@make -s -C ./build/make/ -f qt-multimediaplugins.mk

qt-multimediaplugins-clean: qt-moc-clean qt-rcc-clean qt-uic-clean
	+@make -s -C ./build/make/ -f qt-multimediaplugins.mk clean

qt-multimediawidgets: qt-moc qt-rcc qt-uic
	+@make -s -C ./build/make/ -f qt-multimediawidgets.mk

qt-multimediawidgets-clean: qt-moc-clean qt-rcc-clean qt-uic-clean
	+@make -s -C ./build/make/ -f qt-multimediawidgets.mk clean

qt-network: qt-moc qt-rcc qt-uic
	+@make -s -C ./build/make/ -f qt-network.mk

qt-network-clean: qt-moc-clean qt-rcc-clean qt-uic-clean
	+@make -s -C ./build/make/ -f qt-network.mk clean

qt-opengl: qt-moc qt-rcc qt-uic
	+@make -s -C ./build/make/ -f qt-opengl.mk

qt-opengl-clean: qt-moc-clean qt-rcc-clean qt-uic-clean
	+@make -s -C ./build/make/ -f qt-opengl.mk clean

qt-printsupport: qt-moc qt-rcc qt-uic
	+@make -s -C ./build/make/ -f qt-printsupport.mk

qt-printsupport-clean: qt-moc-clean qt-rcc-clean qt-uic-clean
	+@make -s -C ./build/make/ -f qt-printsupport.mk clean

qt-rcc: zlib qt-bootstrap
	+@make -s -C ./build/make/ -f qt-rcc.mk

qt-rcc-clean: zlib-clean qt-bootstrap-clean
	+@make -s -C ./build/make/ -f qt-rcc.mk clean

qt-serialport: qt-moc qt-rcc qt-uic
	+@make -s -C ./build/make/ -f qt-serialport.mk

qt-serialport-clean: qt-moc-clean qt-rcc-clean qt-uic-clean
	+@make -s -C ./build/make/ -f qt-serialport.mk clean

qt-uic: zlib qt-bootstrap
	+@make -s -C ./build/make/ -f qt-uic.mk

qt-uic-clean: zlib-clean qt-bootstrap-clean
	+@make -s -C ./build/make/ -f qt-uic.mk clean

qt-websockets: qt-moc qt-rcc qt-uic
	+@make -s -C ./build/make/ -f qt-websockets.mk

qt-websockets-clean: qt-moc-clean qt-rcc-clean qt-uic-clean
	+@make -s -C ./build/make/ -f qt-websockets.mk clean

qt-widgets: qt-moc qt-rcc qt-uic
	+@make -s -C ./build/make/ -f qt-widgets.mk

qt-widgets-clean: qt-moc-clean qt-rcc-clean qt-uic-clean
	+@make -s -C ./build/make/ -f qt-widgets.mk clean

qt-xml: qt-moc qt-rcc qt-uic
	+@make -s -C ./build/make/ -f qt-xml.mk

qt-xml-clean: qt-moc-clean qt-rcc-clean qt-uic-clean
	+@make -s -C ./build/make/ -f qt-xml.mk clean

singleapplication: qt-moc qt-rcc qt-uic
	+@make -s -C ./build/make/ -f singleapplication.mk

singleapplication-clean: qt-moc-clean qt-rcc-clean qt-uic-clean
	+@make -s -C ./build/make/ -f singleapplication.mk clean

tinyaes:
	+@make -s -C ./build/make/ -f tinyaes.mk

tinyaes-clean:
	+@make -s -C ./build/make/ -f tinyaes.mk clean

tinyobjloader:
	+@make -s -C ./build/make/ -f tinyobjloader.mk

tinyobjloader-clean:
	+@make -s -C ./build/make/ -f tinyobjloader.mk clean

tinyxml:
	+@make -s -C ./build/make/ -f tinyxml.mk

tinyxml-clean:
	+@make -s -C ./build/make/ -f tinyxml.mk clean

tonic:
	+@make -s -C ./build/make/ -f tonic.mk

tonic-clean:
	+@make -s -C ./build/make/ -f tonic.mk clean

wavelib:
	+@make -s -C ./build/make/ -f wavelib.mk

wavelib-clean:
	+@make -s -C ./build/make/ -f wavelib.mk clean

zlib:
	+@make -s -C ./build/make/ -f zlib.mk

zlib-clean:
	+@make -s -C ./build/make/ -f zlib.mk clean

# qt-tools
qt-tools: qt-moc qt-rcc qt-uic

# engine-dependencies
engine-dependencies: brainflow \
  brainflow-boardcontroller \
  edflib \
  oscpack \
  kissfft \
  zlib

# combined
--base: brainflow \
  brainflow-boardcontroller \
  brainflow-datahandler \
  brainflow-ganglion-bglib \
  brainflow-ml \
  double-conversion \
  dspfilters \
  edflib \
  freetype \
  harfbuzz \
  harfbuzz-ng \
  kissfft \
  libcrypto \
  libjpeg \
  libpng \
  libssl \
  libsvm \
  minizip \
  opencv-core \
  opencv-imgcodecs \
  opencv-imgproc \
  opencv-videoio \
  oscpack \
  pcre2 \
  qt-bluetooth \
  qt-bootstrap \
  qt-concurrent \
  qt-core \
  qt-eventdispatchers \
  qt-fontdatabases \
  qt-gamepad \
  qt-gui \
  qt-moc \
  qt-multimedia \
  qt-multimediaplugins \
  qt-multimediawidgets \
  qt-network \
  qt-opengl \
  qt-printsupport \
  qt-rcc \
  qt-serialport \
  qt-uic \
  qt-websockets \
  qt-widgets \
  qt-xml \
  singleapplication \
  tinyaes \
  tinyobjloader \
  tinyxml \
  tonic \
  wavelib \
  zlib 

--base-clean: brainflow-clean \
  brainflow-boardcontroller-clean \
  brainflow-datahandler-clean \
  brainflow-ganglion-bglib-clean \
  brainflow-ml-clean \
  double-conversion-clean \
  dspfilters-clean \
  edflib-clean \
  freetype-clean \
  harfbuzz-clean \
  harfbuzz-ng-clean \
  kissfft-clean \
  libcrypto-clean \
  libjpeg-clean \
  libpng-clean \
  libssl-clean \
  libsvm-clean \
  minizip-clean \
  opencv-core-clean \
  opencv-imgcodecs-clean \
  opencv-imgproc-clean \
  opencv-videoio-clean \
  oscpack-clean \
  pcre2-clean \
  qt-bluetooth-clean \
  qt-bootstrap-clean \
  qt-concurrent-clean \
  qt-core-clean \
  qt-eventdispatchers-clean \
  qt-fontdatabases-clean \
  qt-gamepad-clean \
  qt-gui-clean \
  qt-moc-clean \
  qt-multimedia-clean \
  qt-multimediaplugins-clean \
  qt-multimediawidgets-clean \
  qt-network-clean \
  qt-opengl-clean \
  qt-printsupport-clean \
  qt-rcc-clean \
  qt-serialport-clean \
  qt-uic-clean \
  qt-websockets-clean \
  qt-widgets-clean \
  qt-xml-clean \
  singleapplication-clean \
  tinyaes-clean \
  tinyobjloader-clean \
  tinyxml-clean \
  tonic-clean \
  wavelib-clean \
  zlib-clean 

##################################################################################
# WINDOWS
##################################################################################
ifeq ($(TARGET_OS),win)

brainflow-gforcesdkcxx-wrapper: brainflow
	+@make -s -C ./build/make/ -f brainflow-gforcesdkcxx-wrapper.mk

brainflow-gforcesdkcxx-wrapper-clean: brainflow-clean
	+@make -s -C ./build/make/ -f brainflow-gforcesdkcxx-wrapper.mk clean

qt-platform-windows: qt-moc qt-rcc qt-uic
	+@make -s -C ./build/make/ -f qt-platform-windows.mk

qt-platform-windows-clean: qt-moc-clean qt-rcc-clean qt-uic-clean
	+@make -s -C ./build/make/ -f qt-platform-windows.mk clean

all: --base brainflow-gforcesdkcxx-wrapper qt-platform-windows
clean: --base-clean brainflow-gforcesdkcxx-wrapper-clean qt-platform-windows-clean

endif

##################################################################################
# OSX
##################################################################################
ifeq ($(TARGET_OS),osx)

qt-platform-cocoa: qt-moc qt-rcc qt-uic
	+@make -s -C ./build/make/ -f qt-platform-cocoa.mk

qt-platform-cocoa-clean: qt-moc-clean qt-rcc-clean qt-uic-clean
	+@make -s -C ./build/make/ -f qt-platform-cocoa.mk clean

all: --base qt-platform-cocoa
clean: --base-clean qt-platform-cocoa-clean

endif

##################################################################################
# LINUX
##################################################################################
ifeq ($(TARGET_OS),linux)

fontconfig:
	+@make -s -C ./build/make/ -f fontconfig.mk

fontconfig-clean:
	+@make -s -C ./build/make/ -f fontconfig.mk clean

qt-dbus: qt-moc qt-rcc qt-uic
	+@make -s -C ./build/make/ -f qt-dbus.mk

qt-dbus-clean: qt-moc-clean qt-rcc-clean qt-uic-clean
	+@make -s -C ./build/make/ -f qt-dbus.mk clean

qt-platform-xcb: qt-moc qt-rcc qt-uic
	+@make -s -C ./build/make/ -f qt-platform-xcb.mk

qt-platform-xcb-clean: qt-moc-clean qt-rcc-clean qt-uic-clean
	+@make -s -C ./build/make/ -f qt-platform-xcb.mk clean

all: --base fontconfig qt-dbus qt-platform-xcb
clean: --base-clean fontconfig-clean qt-dbus-clean qt-platform-xcb-clean

endif

##################################################################################
# ANDROID
##################################################################################
ifeq ($(TARGET_OS),android)

all: --base
clean: --base-clean

endif

##################################################################################
# iOS
##################################################################################
ifeq ($(TARGET_OS),ios)

all: --base
clean: --base-clean

endif

##################################################################################

.PHONY : all
.DEFAULT_GOAL := all
