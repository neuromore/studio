########################################################
# Detect Host OS, CPU and CORES
########################################################

# Windows (no uname)
ifeq ($(OS),Windows_NT)
	DETECTED_OS := win
	DETECTED_EXTBIN := .exe

	# Detect platform arch
	ifneq ($(PROCESSOR_ARCHITEW6432),)
		DETECTED_ARCH := $(PROCESSOR_ARCHITEW6432)
	else
		DETECTED_ARCH := $(PROCESSOR_ARCHITECTURE)
	endif
	
	# Map platform arch to our x64/x86
	ifeq ($(DETECTED_ARCH),AMD64)
		DETECTED_ARCH := x64
	else
		DETECTED_ARCH := x86
	endif
	
	# Detect # of cores
	DETECTED_CORES := $(NUMBER_OF_PROCESSORS)

# Linux/OSX (anything with uname)
else
	DETECTED_OS := $(shell uname)
	DETECTED_EXTBIN :=
		
	# Map os name to our linux/osx
	ifeq ($(DETECTED_OS),Linux)
		DETECTED_OS := linux
	endif
	ifeq ($(DETECTED_OS),Darwin)
		DETECTED_OS := osx
	endif
	
	# Map platform arch to our x64/x86
	ifeq ($(shell uname -m),x86_64)
		DETECTED_ARCH := x64
	else
		DETECTED_ARCH := x86
	endif
	
	# Detect # of cores
	ifeq ($(DETECTED_OS),linux)
		DETECTED_CORES := $(shell nproc)
	endif
	ifeq ($(DETECTED_OS),osx)
		DETECTED_CORES := $(shell sysctl -n hw.ncpu)
	endif
endif

# Debug by default
ifeq ($(MODE),)
MODE = debug
endif

# Debug vs. Release
ifeq ($(MODE),debug)
SUFFIX = _d
else
SUFFIX =
endif

# Qt Tools
QTMOC = bin/$(DETECTED_OS)-$(DETECTED_ARCH)/qt-moc$(SUFFIX)$(DETECTED_EXTBIN)
QTUIC = bin/$(DETECTED_OS)-$(DETECTED_ARCH)/qt-uic$(SUFFIX)$(DETECTED_EXTBIN)
QTRCC = bin/$(DETECTED_OS)-$(DETECTED_ARCH)/qt-rcc$(SUFFIX)$(DETECTED_EXTBIN)

# JAVA JDK
ifeq ($(DETECTED_OS),osx)
	JAVA_HOME :=`/usr/libexec/java_home`
endif

JAR   = "$(JAVA_HOME)/bin/jar$(DETECTED_EXTBIN)"
JAVAC = "$(JAVA_HOME)/bin/javac$(DETECTED_EXTBIN)"
