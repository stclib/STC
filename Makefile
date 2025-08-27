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

CFLAGS    ?= -std=c11 -Iinclude -O3 -MMD -Werror -Wpedantic -Wall -Wextra -Wconversion -Wno-missing-field-initializers
CXXFLAGS  ?= -std=c++20 -Iinclude -O3 -MMD -Wall
ifeq ($(CC),tcc)
  AR_RCS  ?= tcc -ar rcs
else
  AR_RCS  ?= ar rcs
endif
MKDIR_P   ?= mkdir -p
RM_F      ?= rm -f

ifeq ($(OS),Windows_NT)
	DOTEXE := .exe
	BUILDDIR := build/Windows_$(CC)
	LDFLAGS += -fopenmp
else
#	CC_VER := $(shell $(CC) -dumpversion | cut -f1 -d.)
	BUILDDIR := build/$(shell uname)_$(CC)
	LDFLAGS += -lm
    # Check preprocessor macros to detect clang compiler
	IS_CLANG_COMPILER := $(shell echo '' | $(CC) -dM -E -x c - | grep -q '__clang__' > /dev/null 2>&1 && echo 1 || echo 0)
	ifneq ($(IS_CLANG_COMPILER),1)
	  LDFLAGS += -fopenmp
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
EX_EXES   := $(EX_SRCS:%.c=$(BUILDDIR)/%$(DOTEXE))

TEST_SRCS   := $(wildcard tests/*_test.c) tests/main.c
TEST_OBJS   := $(TEST_SRCS:%.c=$(OBJ_DIR)/%.o)
TEST_DEPS   := $(TEST_SRCS:%.c=$(OBJ_DIR)/%.d)
TEST_EXE    := $(BUILDDIR)/tests/test_all$(DOTEXE)

PROGRAMS	:= $(EX_EXES) $(TEST_EXE)

fast:
	@$(MAKE) -j --no-print-directory all

all: $(PROGRAMS)
	@echo
	@$(TEST_EXE)

$(PROGRAMS): $(LIB_PATH)

clean:
	@$(RM_F) $(LIB_OBJS) $(EX_OBJS) $(TEST_OBJS) $(LIB_DEPS) $(EX_DEPS) $(TEST_DEPS) $(LIB_PATH) $(EX_EXES) $(TEST_EXE)
	@echo "Cleaned"

distclean:
	@$(RM_F) -r build
	@echo "All Cleaned"

lib: $(LIB_PATH)
	@echo

$(LIB_PATH): $(LIB_OBJS)
	@printf "\r\e[2K%s" "$(AR_RCS) $@"
	@$(AR_RCS) $@ $(LIB_OBJS)

$(OBJ_DIR)/%.o: %.c Makefile
	@$(MKDIR_P) $(@D)
	@printf "\r\e[2K%s" "$(CC) $(<F) -o $@"
	@$(CC) $< -c -o $@ $(CFLAGS)

$(OBJ_DIR)/%.o: %.cpp Makefile
	@$(MKDIR_P) $(@D)
	@printf "\r\e[2K%s" "$(CXX) $(<F) -o $@"
	@$(CXX) $< -c -o $@ $(CXXFLAGS)

$(OBJ_DIR)/%$(DOTEXE): %.c $(LIB_PATH) Makefile
	@$(MKDIR_P) $(@D)
	@printf "\r\e[2K%s" "$(CC) $(<F) -o $@"
	@$(CC) -o $@ $(CFLAGS) -s $< $(LDFLAGS) -L$(BUILDDIR) -l$(LIB_NAME)

$(TEST_EXE): $(TEST_OBJS)
	@printf "\r\e[2K%s" "$(CC) -o $@"
	@$(CC) -o $@ $(TEST_OBJS) -s $(LDFLAGS) -L$(BUILDDIR) -l$(LIB_NAME)


.SECONDARY: $(EX_OBJS) # Prevent deleting objs after building
.PHONY: fast all clean distclean lib

-include $(LIB_DEPS) $(TEST_DEPS) $(EX_DEPS)
