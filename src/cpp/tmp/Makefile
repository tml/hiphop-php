
PROJECT_NAME = test
PROJECT_ROOT = $(HPHP_HOME)

AUTO_SOURCES_RECURSIVE = 1
INTERMEDIATE_FILES = main.cpp main.php
INTERMEDIATE_DIRS += cls php sys

PROGRAMS = $(shell ls | grep Test)

CPPFLAGS += -I.
LIBS =  \
        $(LIB_DIR)/libhphp_runtime.a \
        $(ALL_LIBS)

include $(PROJECT_ROOT)/src/rules.mk
TARGETS = $(PROGRAMS)

all: $(TARGETS)
