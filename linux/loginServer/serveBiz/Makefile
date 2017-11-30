#****************************************************************************
#
# Makefile for socketlite
# bolide zhang
# bolidezhang@gmail.com
#
# This is a GNU make (gmake) makefile
#****************************************************************************

# DEBUG can be set to YES to include debugging info, or NO otherwise
DEBUG          := YES

# PROFILE can be set to YES to include profiling info, or NO otherwise
PROFILE        := NO

# USE_STL can be used to turn on STL support. NO, then STL
# will not be used. YES will include the STL files.
USE_STL := YES

# WIN32_ENV
WIN32_ENV := YES
#****************************************************************************

CC     := gcc
CXX    := g++
LD     := g++
AR     := ar rc
RANLIB := ranlib

# ifeq (YES, ${WIN32_ENV})
#   RM     := del
# else
#   RM     := rm -f
# endif

DEBUG_CFLAGS     := -Wall -Wno-format -g -DDEBUG
RELEASE_CFLAGS   := -Wall -Wno-unknown-pragmas -Wno-format -O2

DEBUG_CXXFLAGS   := ${DEBUG_CFLAGS}
RELEASE_CXXFLAGS := ${RELEASE_CFLAGS}

DEBUG_LDFLAGS    := -g
RELEASE_LDFLAGS  := -O3

ifeq (YES, ${DEBUG})
   CFLAGS       := ${DEBUG_CFLAGS}
   CXXFLAGS     := ${DEBUG_CXXFLAGS}
   LDFLAGS      := ${DEBUG_LDFLAGS}
else
   CFLAGS       := ${RELEASE_CFLAGS}
   CXXFLAGS     := ${RELEASE_CXXFLAGS}
   LDFLAGS      := ${RELEASE_LDFLAGS}
endif

ifeq (YES, ${PROFILE})
   CFLAGS   := ${CFLAGS} -pg -O3
   CXXFLAGS := ${CXXFLAGS} -pg -O3
   LDFLAGS  := ${LDFLAGS} -pg
endif

#****************************************************************************
# Preprocessor directives
#****************************************************************************

ifeq (YES, ${USE_STL})
  DEFS := -DUSE_STL
else
  DEFS :=
endif

#****************************************************************************
# Include paths
#****************************************************************************

INCS := -I./internal -I. -I./robin -I../socketLite -I../thrift/src/thrift -I./protocol/thrift
LIBS := 

#****************************************************************************
# Makefile code common to all platforms
#****************************************************************************

CFLAGS   := ${CFLAGS}   ${DEFS}
CXXFLAGS := ${CXXFLAGS} ${DEFS}

#****************************************************************************
# Targets of the build
#****************************************************************************
OUTPUT_DIR := objs
TARGET := libservebiz.a

all: ${TARGET}


#****************************************************************************
# Source files
#****************************************************************************

INTERNAL_DIR = ./internal
ROBIN_DIR = ./robin
PROTOCOL_DIR = ./protocol/thrift

SRCS := $(INTERNAL_DIR)/center_server_handler.cpp \
	$(INTERNAL_DIR)/dbproxy_handler.cpp \
	$(INTERNAL_DIR)/login_server_handler.cpp \
	$(INTERNAL_DIR)/main_stage_handler.cpp \
	$(INTERNAL_DIR)/serialize.cpp \
	$(INTERNAL_DIR)/tcp_server_manager.cpp \
	$(PROTOCOL_DIR)/login_server_constants.cpp \
	$(PROTOCOL_DIR)/login_server_types.cpp \
	$(PROTOCOL_DIR)/room_server_constants.cpp \
	$(PROTOCOL_DIR)/room_server_types.cpp \
	$(PROTOCOL_DIR)/center_server_constants.cpp \
	$(PROTOCOL_DIR)/center_server_types.cpp \
	$(PROTOCOL_DIR)/common_constants.cpp \
	$(PROTOCOL_DIR)/common_types.cpp \
	$(PROTOCOL_DIR)/dbproxy_server_constants.cpp \
	$(PROTOCOL_DIR)/dbproxy_server_types.cpp \
	serveBiz.cpp

# Add on the sources for libraries
SRCS := ${SRCS}

OBJS := $(addsuffix .o,$(basename ${SRCS}))

#****************************************************************************
# Output
#****************************************************************************

${TARGET}: ${OBJS}
#	${LD} -o $@ ${LDFLAGS} ${OBJS} ${LIBS} ${EXTRA_LIBS}
	$(AR) $@ $(OBJS)
	cp ${TARGET} ../lib
#****************************************************************************
# common rules
#****************************************************************************

# Rules for compiling source files to object files
%.o : %.cpp
	${CXX} -c ${CXXFLAGS} ${INCS} $< -o $@

%.o : %.c
	${CC} -c ${CFLAGS} ${INCS} $< -o $@

dist:
	bash makedistlinux

clean:
	${RM} core ${OBJS} ${TARGET}

depend:
	#makedepend ${INCS} ${SRCS}

%.o: %.h
