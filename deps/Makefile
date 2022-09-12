
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
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f brainflow.mk -j $(DETECTED_CORES)

brainflow-clean:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f brainflow.mk clean -j $(DETECTED_CORES)

brainflow-boardcontroller:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f brainflow-boardcontroller.mk -j $(DETECTED_CORES)

brainflow-boardcontroller-clean:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f brainflow-boardcontroller.mk clean -j $(DETECTED_CORES)

brainflow-datahandler:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f brainflow-datahandler.mk -j $(DETECTED_CORES)

brainflow-datahandler-clean:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f brainflow-datahandler.mk clean -j $(DETECTED_CORES)

brainflow-ganglion-bglib: brainflow
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f brainflow-ganglion-bglib.mk -j $(DETECTED_CORES)

brainflow-ganglion-bglib-clean: brainflow-clean
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f brainflow-ganglion-bglib.mk clean -j $(DETECTED_CORES)

brainflow-ml:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f brainflow-ml.mk -j $(DETECTED_CORES)

brainflow-ml-clean:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f brainflow-ml.mk clean -j $(DETECTED_CORES)

double-conversion:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f double-conversion.mk -j $(DETECTED_CORES)

double-conversion-clean:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f double-conversion.mk clean -j $(DETECTED_CORES)

dspfilters:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f dspfilters.mk -j $(DETECTED_CORES)

dspfilters-clean:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f dspfilters.mk clean -j $(DETECTED_CORES)

edflib:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f edflib.mk -j $(DETECTED_CORES)

edflib-clean:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f edflib.mk clean -j $(DETECTED_CORES)

freetype:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f freetype.mk -j $(DETECTED_CORES)

freetype-clean:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f freetype.mk clean -j $(DETECTED_CORES)

harfbuzz:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f harfbuzz.mk -j $(DETECTED_CORES)

harfbuzz-clean:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f harfbuzz.mk clean -j $(DETECTED_CORES)

harfbuzz-ng:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f harfbuzz-ng.mk -j $(DETECTED_CORES)

harfbuzz-ng-clean:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f harfbuzz-ng.mk clean -j $(DETECTED_CORES)

kissfft:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f kissfft.mk -j $(DETECTED_CORES)

kissfft-clean:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f kissfft.mk clean -j $(DETECTED_CORES)

libcrypto:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f libcrypto.mk -j $(DETECTED_CORES)

libcrypto-clean:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f libcrypto.mk clean -j $(DETECTED_CORES)

libjpeg:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f libjpeg.mk -j $(DETECTED_CORES)

libjpeg-clean:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f libjpeg.mk clean -j $(DETECTED_CORES)

libpng:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f libpng.mk -j $(DETECTED_CORES)

libpng-clean:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f libpng.mk clean -j $(DETECTED_CORES)

libssl:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f libssl.mk -j $(DETECTED_CORES)

libssl-clean:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f libssl.mk clean -j $(DETECTED_CORES)

libsvm:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f libsvm.mk -j $(DETECTED_CORES)

libsvm-clean:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f libsvm.mk clean -j $(DETECTED_CORES)

minizip:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f minizip.mk -j $(DETECTED_CORES)

minizip-clean:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f minizip.mk clean -j $(DETECTED_CORES)

opencv-core:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f opencv-core.mk -j $(DETECTED_CORES)

opencv-core-clean:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f opencv-core.mk clean -j $(DETECTED_CORES)

opencv-imgcodecs:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f opencv-imgcodecs.mk -j $(DETECTED_CORES)

opencv-imgcodecs-clean:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f opencv-imgcodecs.mk clean -j $(DETECTED_CORES)

opencv-imgproc:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f opencv-imgproc.mk -j $(DETECTED_CORES)

opencv-imgproc-clean:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f opencv-imgproc.mk clean -j $(DETECTED_CORES)

opencv-videoio:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f opencv-videoio.mk -j $(DETECTED_CORES)

opencv-videoio-clean:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f opencv-videoio.mk clean -j $(DETECTED_CORES)

oscpack:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f oscpack.mk -j $(DETECTED_CORES)

oscpack-clean:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f oscpack.mk clean -j $(DETECTED_CORES)

pcre2:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f pcre2.mk -j $(DETECTED_CORES)

pcre2-clean:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f pcre2.mk clean -j $(DETECTED_CORES)

qt-bluetooth: qt-moc qt-rcc qt-uic
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-bluetooth.mk -j $(DETECTED_CORES)

qt-bluetooth-clean: qt-moc-clean qt-rcc-clean qt-uic-clean
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-bluetooth.mk clean -j $(DETECTED_CORES)

qt-bootstrap:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-bootstrap.mk -j $(DETECTED_CORES)

qt-bootstrap-clean:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-bootstrap.mk clean -j $(DETECTED_CORES)

qt-concurrent: qt-moc qt-rcc qt-uic
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-concurrent.mk -j $(DETECTED_CORES)

qt-concurrent-clean: qt-moc-clean qt-rcc-clean qt-uic-clean
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-concurrent.mk clean -j $(DETECTED_CORES)

qt-core: qt-moc qt-rcc qt-uic
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-core.mk -j $(DETECTED_CORES)

qt-core-clean: qt-moc-clean qt-rcc-clean qt-uic-clean
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-core.mk clean -j $(DETECTED_CORES)

qt-eventdispatchers: qt-moc qt-rcc qt-uic
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-eventdispatchers.mk -j $(DETECTED_CORES)

qt-eventdispatchers-clean: qt-moc-clean qt-rcc-clean qt-uic-clean
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-eventdispatchers.mk clean -j $(DETECTED_CORES)

qt-fontdatabases: qt-moc qt-rcc qt-uic
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-fontdatabases.mk -j $(DETECTED_CORES)

qt-fontdatabases-clean: qt-moc-clean qt-rcc-clean qt-uic-clean
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-fontdatabases.mk clean -j $(DETECTED_CORES)

qt-gamepad: qt-moc qt-rcc qt-uic
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-gamepad.mk -j $(DETECTED_CORES)

qt-gamepad-clean: qt-moc-clean qt-rcc-clean qt-uic-clean
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-gamepad.mk clean -j $(DETECTED_CORES)

qt-gui: qt-moc qt-rcc qt-uic
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-gui.mk -j $(DETECTED_CORES)

qt-gui-clean: qt-moc-clean qt-rcc-clean qt-uic-clean
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-gui.mk clean -j $(DETECTED_CORES)

qt-moc: zlib qt-bootstrap
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-moc.mk -j $(DETECTED_CORES)

qt-moc-clean: zlib-clean qt-bootstrap-clean
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-moc.mk clean -j $(DETECTED_CORES)

qt-multimedia: qt-moc qt-rcc qt-uic
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-multimedia.mk -j $(DETECTED_CORES)

qt-multimedia-clean: qt-moc-clean qt-rcc-clean qt-uic-clean
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-multimedia.mk clean -j $(DETECTED_CORES)

qt-multimediaplugins: qt-moc qt-rcc qt-uic
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-multimediaplugins.mk -j $(DETECTED_CORES)

qt-multimediaplugins-clean: qt-moc-clean qt-rcc-clean qt-uic-clean
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-multimediaplugins.mk clean -j $(DETECTED_CORES)

qt-multimediawidgets: qt-moc qt-rcc qt-uic
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-multimediawidgets.mk -j $(DETECTED_CORES)

qt-multimediawidgets-clean: qt-moc-clean qt-rcc-clean qt-uic-clean
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-multimediawidgets.mk clean -j $(DETECTED_CORES)

qt-network: qt-moc qt-rcc qt-uic
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-network.mk -j $(DETECTED_CORES)

qt-network-clean: qt-moc-clean qt-rcc-clean qt-uic-clean
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-network.mk clean -j $(DETECTED_CORES)

qt-opengl: qt-moc qt-rcc qt-uic
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-opengl.mk -j $(DETECTED_CORES)

qt-opengl-clean: qt-moc-clean qt-rcc-clean qt-uic-clean
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-opengl.mk clean -j $(DETECTED_CORES)

qt-printsupport: qt-moc qt-rcc qt-uic
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-printsupport.mk -j $(DETECTED_CORES)

qt-printsupport-clean: qt-moc-clean qt-rcc-clean qt-uic-clean
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-printsupport.mk clean -j $(DETECTED_CORES)

qt-rcc: zlib qt-bootstrap
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-rcc.mk -j $(DETECTED_CORES)

qt-rcc-clean: zlib-clean qt-bootstrap-clean
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-rcc.mk clean -j $(DETECTED_CORES)

qt-serialport: qt-moc qt-rcc qt-uic
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-serialport.mk -j $(DETECTED_CORES)

qt-serialport-clean: qt-moc-clean qt-rcc-clean qt-uic-clean
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-serialport.mk clean -j $(DETECTED_CORES)

qt-uic: zlib qt-bootstrap
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-uic.mk -j $(DETECTED_CORES)

qt-uic-clean: zlib-clean qt-bootstrap-clean
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-uic.mk clean -j $(DETECTED_CORES)

qt-websockets: qt-moc qt-rcc qt-uic
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-websockets.mk -j $(DETECTED_CORES)

qt-websockets-clean: qt-moc-clean qt-rcc-clean qt-uic-clean
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-websockets.mk clean -j $(DETECTED_CORES)

qt-widgets: qt-moc qt-rcc qt-uic
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-widgets.mk -j $(DETECTED_CORES)

qt-widgets-clean: qt-moc-clean qt-rcc-clean qt-uic-clean
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-widgets.mk clean -j $(DETECTED_CORES)

qt-xml: qt-moc qt-rcc qt-uic
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-xml.mk -j $(DETECTED_CORES)

qt-xml-clean: qt-moc-clean qt-rcc-clean qt-uic-clean
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-xml.mk clean -j $(DETECTED_CORES)

singleapplication: qt-moc qt-rcc qt-uic
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f singleapplication.mk -j $(DETECTED_CORES)

singleapplication-clean: qt-moc-clean qt-rcc-clean qt-uic-clean
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f singleapplication.mk clean -j $(DETECTED_CORES)

tinyaes:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f tinyaes.mk -j $(DETECTED_CORES)

tinyaes-clean:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f tinyaes.mk clean -j $(DETECTED_CORES)

tinyobjloader:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f tinyobjloader.mk -j $(DETECTED_CORES)

tinyobjloader-clean:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f tinyobjloader.mk clean -j $(DETECTED_CORES)

tinyxml:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f tinyxml.mk -j $(DETECTED_CORES)

tinyxml-clean:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f tinyxml.mk clean -j $(DETECTED_CORES)

wavelib:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f wavelib.mk -j $(DETECTED_CORES)

wavelib-clean:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f wavelib.mk clean -j $(DETECTED_CORES)

zlib:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f zlib.mk -j $(DETECTED_CORES)

zlib-clean:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f zlib.mk clean -j $(DETECTED_CORES)

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
  wavelib-clean \
  zlib-clean 

##################################################################################
# WINDOWS
##################################################################################
ifeq ($(TARGET_OS),win)

brainflow-gforcesdkcxx-wrapper: brainflow
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f brainflow-gforcesdkcxx-wrapper.mk -j $(DETECTED_CORES)

brainflow-gforcesdkcxx-wrapper-clean: brainflow-clean
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f brainflow-gforcesdkcxx-wrapper.mk clean -j $(DETECTED_CORES)

qt-platform-windows: qt-moc qt-rcc qt-uic
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-platform-windows.mk -j $(DETECTED_CORES)

qt-platform-windows-clean: qt-moc-clean qt-rcc-clean qt-uic-clean
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-platform-windows.mk clean -j $(DETECTED_CORES)

all: --base brainflow-gforcesdkcxx-wrapper qt-platform-windows
clean: --base-clean brainflow-gforcesdkcxx-wrapper-clean qt-platform-windows-clean

endif

##################################################################################
# OSX
##################################################################################
ifeq ($(TARGET_OS),osx)

qt-platform-cocoa: qt-moc qt-rcc qt-uic
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-platform-cocoa.mk -j $(DETECTED_CORES)

qt-platform-cocoa-clean: qt-moc-clean qt-rcc-clean qt-uic-clean
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-platform-cocoa.mk clean -j $(DETECTED_CORES)

all: --base qt-platform-cocoa
clean: --base-clean qt-platform-cocoa-clean

endif

##################################################################################
# LINUX
##################################################################################
ifeq ($(TARGET_OS),linux)

fontconfig:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f fontconfig.mk -j $(DETECTED_CORES)

fontconfig-clean:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f fontconfig.mk clean -j $(DETECTED_CORES)

qt-dbus: qt-moc qt-rcc qt-uic
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-dbus.mk -j $(DETECTED_CORES)

qt-dbus-clean: qt-moc-clean qt-rcc-clean qt-uic-clean
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-dbus.mk clean -j $(DETECTED_CORES)

qt-platform-xcb: qt-moc qt-rcc qt-uic
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-platform-xcb.mk -j $(DETECTED_CORES)

qt-platform-xcb-clean: qt-moc-clean qt-rcc-clean qt-uic-clean
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f qt-platform-xcb.mk clean -j $(DETECTED_CORES)

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
