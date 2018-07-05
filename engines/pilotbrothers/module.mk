MODULE := engines/pilotbrothers

MODULE_OBJS = \
    detection.o \
    file.o \
	pilotbrothers.o \

# This module can be built as a plugin
ifeq ($(ENABLE_PILOTBROTHERS), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk
