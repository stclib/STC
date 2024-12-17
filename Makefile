# Makefile for STC. Linux/Mac + Windows
# On Windows, the makefile requires mkdir and rm,
# which are found in C:\Program Files\Git\usr\bin.

ifeq ($(OS),Windows_NT)
	EXE := .exe
	BUILDDIR := build/Windows
else
	BUILDDIR := build/$(shell uname)
endif

CC        ?= gcc
CWARNS    ?= -MMD -Werror -Wimplicit-fallthrough=3 -Wno-missing-field-initializers
CFLAGS 	  ?= -Iinclude -O2 -std=c11 -Wpedantic -Wall -Wextra
CXX       ?= g++
CXXFLAGS  ?= -std=c++20 -O2 -MMD -Iinclude
LDFLAGS   ?=
AR_RCS    := ar rcs
MKDIR_P   := mkdir -p
RM_F      := rm -f

OBJ_DIR   := $(BUILDDIR)

LIB_NAME  := stc
LIB_LIST  := cstr_core cstr_io cstr_utf8 cregex csview cspan fmt random stc_core
LIB_SRCS  := $(LIB_LIST:%=src/%.c)
LIB_OBJS  := $(LIB_SRCS:%.c=$(OBJ_DIR)/%.o)
LIB_DEPS  := $(LIB_SRCS:%.c=$(OBJ_DIR)/%.d)
LIB_PATH  := $(BUILDDIR)/lib$(LIB_NAME).a

EX_SRCS   := $(wildcard misc/examples/*/*.c)
EX_OBJS   := $(EX_SRCS:%.c=$(OBJ_DIR)/%.o)
EX_DEPS   := $(EX_SRCS:%.c=$(OBJ_DIR)/%.d)
EX_EXES   := $(EX_SRCS:%.c=$(OBJ_DIR)/%$(EXE))

fast:
	@$(MAKE) -j --no-print-directory all

all: $(LIB_PATH) $(EX_EXES)
	@echo

clean:
	@$(RM_F) $(LIB_OBJS) $(EX_OBJS) $(LIB_DEPS) $(EX_DEPS) $(LIB_PATH) $(EX_EXES)
	@echo Cleaned

distclean:
	$(RM_F) $(OBJ_DIR)

lib: $(LIB_PATH)

$(LIB_PATH): $(LIB_OBJS)
	@printf "\n%s" "$(AR_RCS) $@"
	@$(AR_RCS) $@ $(LIB_OBJS)
	@echo ""

$(OBJ_DIR)/%.o: %.c
	@$(MKDIR_P) $(@D)
	@printf "\r\e[2K%s" "$(CC) $< -o $@ -c $(CFLAGS)"
	@$(CC) $< -o $@ -c $(CFLAGS) $(CWARNS)

$(OBJ_DIR)/%.o: %.cpp
	@$(MKDIR_P) $(@D)
	@printf "\r\e[2K%s" "$(CXX) $< -c $(CXXFLAGS) -o $@"
	@$(CXX) $< -o $@ -c $(CXXFLAGS)

$(OBJ_DIR)/%$(EXE): %.c $(LIB_PATH)
	@$(MKDIR_P) $(@D)
	@printf "\r\e[2K%s" "$(CC) $< $(CFLAGS) -o $(@F) -s $(LDFLAGS) -L$(BUILDDIR) -l$(LIB_NAME) -lm"
	@$(CC) $< $(CFLAGS) $(CWARNS) -o $@ -s $(LDFLAGS) -L$(BUILDDIR) -l$(LIB_NAME) -lm

.SECONDARY: $(EX_OBJS) # Prevent deleting objs after building
.PHONY: all clean distclean

-include $(LIB_DEPS) $(EX_DEPS)
