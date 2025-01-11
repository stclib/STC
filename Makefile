# GNU Makefile for STC. Linux/Mac + Windows for gcc, clang and tcc (TinyC).
# On Windows, makefile requires mkdir, rm, and printf, which are all
# found in the C:\Program Files\Git\usr\bin folder.
# Use 'make CC=gcc' (or clang or tcc), or set CC in the environment.
# Currently only release build.

ifeq ($(origin CC),default)
	CC := gcc
endif
ifeq ($(origin CXX),default)
	CXX := g++
endif

CFLAGS 	  ?= -std=c11 -Iinclude -MMD -O3 -Wpedantic -Wall -Wextra -Werror -Wno-missing-field-initializers
CXXFLAGS  ?= -std=c++20 -Iinclude -O3 -MMD -Wall
LDFLAGS   ?=
ifeq ($(CC),tcc)
  AR_RCS  ?= tcc -ar rcs
else
  AR_RCS  ?= ar rcs
endif
MKDIR_P   ?= mkdir -p
RM_F      ?= rm -f

ifeq ($(OS),Windows_NT)
	DOTEXE := .exe
	BUILDDIR := bld_Windows/$(CC)
else
#	CC_VER := $(shell $(CC) -dumpversion | cut -f1 -d.)
	BUILDDIR := bld_$(shell uname)/$(CC)
	LDFLAGS += -lm
	ifneq ($(CC),clang)
	  CFLAGS += -Wno-clobbered
	endif
endif

OBJ_DIR   := $(BUILDDIR)

LIB_NAME  := stc
LIB_LIST  := cstr_core cstr_io cstr_utf8 cregex csview cspan fmt random stc_core
LIB_SRCS  := $(LIB_LIST:%=src/%.c)
LIB_OBJS  := $(LIB_SRCS:%.c=$(OBJ_DIR)/%.o)
LIB_DEPS  := $(LIB_SRCS:%.c=$(OBJ_DIR)/%.d)
LIB_PATH  := $(BUILDDIR)/lib$(LIB_NAME).a

EX_SRCS   := $(wildcard examples/*/*.c)
EX_OBJS   := $(EX_SRCS:%.c=$(OBJ_DIR)/%.o)
EX_DEPS   := $(EX_SRCS:%.c=$(OBJ_DIR)/%.d)
EX_EXES   := $(EX_SRCS:%.c=$(OBJ_DIR)/%$(DOTEXE))

TEST_SRCS   := $(wildcard tests/*_test.c) tests/main.c
TEST_OBJS   := $(TEST_SRCS:%.c=$(OBJ_DIR)/%.o)
TEST_DEPS   := $(TEST_SRCS:%.c=$(OBJ_DIR)/%.d)
TEST_EXE    := $(OBJ_DIR)/tests/test_all$(DOTEXE)

PROGRAMS	:= $(EX_EXES) $(TEST_EXE)

fast:
	@$(MAKE) -j --no-print-directory all

all: $(PROGRAMS)
	@echo

$(PROGRAMS): $(LIB_PATH)

clean:
	@$(RM_F) $(LIB_OBJS) $(TEST_OBJS) $(EX_OBJS) $(LIB_DEPS) $(EX_DEPS) $(LIB_PATH) $(EX_EXES) $(TEST_EXE)
	@echo "Cleaned"

distclean:
	$(RM_F) $(OBJ_DIR)

lib: $(LIB_PATH)

$(LIB_PATH): $(LIB_OBJS)
	@printf "\r\e[2K%s" "$(AR_RCS) $@"
	@$(AR_RCS) $@ $(LIB_OBJS)

$(OBJ_DIR)/%.o: %.c
	@$(MKDIR_P) $(@D)
	@printf "\r\e[2K%s" "$(CC) $< -c -o $@"
	@$(CC) $< -c -o $@ $(CFLAGS)

$(OBJ_DIR)/%.o: %.cpp
	@$(MKDIR_P) $(@D)
	@printf "\r\e[2K%s" "$(CXX) $< -c -o $@"
	@$(CXX) $< -c -o $@ $(CXXFLAGS)

$(OBJ_DIR)/%$(DOTEXE): %.c $(LIB_PATH)
	@$(MKDIR_P) $(@D)
	@printf "\r\e[2K%s" "$(CC) -o $(@F) $< -s $(LDFLAGS) -L$(BUILDDIR) -l$(LIB_NAME)"
	@$(CC) -o $@ $(CFLAGS) -s $< $(LDFLAGS) -L$(BUILDDIR) -l$(LIB_NAME)

$(TEST_EXE): $(TEST_OBJS)
	@printf "\r\e[2K%s" "$(CC) -o $@"
	@$(CC) -o $@ $(TEST_OBJS) -s $(LDFLAGS) -L$(BUILDDIR) -l$(LIB_NAME)


.SECONDARY: $(EX_OBJS) # Prevent deleting objs after building
.PHONY: fast all clean distclean lib

-include $(LIB_DEPS) $(EX_DEPS)
