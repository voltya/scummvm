MODULE := engines/composer4

MODULE_OBJS = \
	composer4.o \
	console.o \
	metaengine.o \
	library.o \
	button.o

# This module can be built as a plugin
ifeq ($(ENABLE_COMPOSER4), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk

# Detection objects
DETECT_OBJS += $(MODULE)/detection.o
