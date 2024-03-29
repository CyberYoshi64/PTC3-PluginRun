ifeq ($(strip $(DEVKITPRO)),)
    $(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>devkitPro")
endif

NAME := CYXPlgRun
PROCESS_NAME := CY64-CYX

BUILD_DIR := build
OUTPUT_DIR := output
SOURCE_DIRS := source
INCLUDE_DIRS := $(SOURCE_DIRS) include

LIBRARY_DIRS = $(DEVKITPRO)/portlibs/3ds $(CTRULIB) $(DEVKITPRO)/libctru $(DEVKITPRO)/libcwav $(DEVKITPRO)/libncsnd
LIBRARIES = citro2d citro3d png curl mbedtls mbedx509 mbedcrypto ctru cwav ncsnd z m

VERSION_MAJOR := 0
VERSION_MINOR := 0
VERSION_MICRO := 0

PRODUCT_CODE := CTR-N-CYXZ
UNIQUE_ID := 0x16903

CATEGORY := Application
USE_ON_SD := true

MEMORY_TYPE := Application
SYSTEM_MODE := 64MB
SYSTEM_MODE_EXT := Legacy
CPU_SPEED := 804MHz
ENABLE_L2_CACHE := true

MAIN_RSF := resources/main.rsf
MANUAL_RSF := resources/manual.rsf

MAIN_ROMFS := romfs
MANUAL_ROMFS := romfs-manual

ICON := build/icon.icn
ICON_3DSX := build/icon.smdh

BANNER_AUDIO := resources/audio.bcwav
BANNER_IMAGE := resources/banner.cgfx.png
LOGO := resources/logo.bcma.lz

BUILD_FLAGS := -march=armv6k -mtune=mpcore -mfloat-abi=hard
BUILD_FLAGS_CC := 	-g -Wall -Wno-strict-aliasing -O2 -mword-relocations \
					-fomit-frame-pointer -ffast-math \
					$(ARCH) $(INCLUDE) -D__3DS__ $(BUILD_FLAGS) \
					-DAPP_VERSION_MAJOR=${VERSION_MAJOR} \
					-DAPP_VERSION_MINOR=${VERSION_MINOR} \
					-DAPP_VERSION_MICRO=${VERSION_MICRO}

BUILD_FLAGS_CXX := $(COMMON_FLAGS) -std=gnu++20

include resources/makerules

re : clean .WAIT all
.PHONY: re