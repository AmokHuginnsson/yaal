CWARNING_FLAGS = \
						-Wall \
						-Wcast-align \
						-Wcast-qual \
						-Wconversion \
						-Wendif-labels \
						-Werror \
						-Wmissing-format-attribute \
						-Wmissing-noreturn \
						-Wpointer-arith \
						-Wredundant-decls \
						-Wsign-compare \
						-Wundef \
						-Wwrite-strings
# -Winline makes no sense as inline keyword is only suggestion
#  and actual inlining can change by simply changing compiler version.
CXXWARNING_FLAGS = \
						-Weffc++ \
						-Wold-style-cast \
						-Woverloaded-virtual \
						-Wsign-conversion \
						-Wsign-promo \
						-Wzero-as-null-pointer-constant
# -Waggregate-return makes no sense for C++ as it effectively banishes return by value.
PURE_C_WARNING_FLAGS = \
						-Waggregate-return \
						-Wbad-function-cast \
						-Wmissing-prototypes \
						-Wnested-externs \
						-Wstrict-prototypes \
						-Wtraditional
ifeq ($(HD),1)
	DB = -D__DEBUGGER_BABUNI__
endif
COMPILER_PRIME_FLAGS = -fmessage-length=0 -Wno-invalid-source-encoding -Wno-strict-overflow -pipe -D_GNU_SOURCE -D_REENTRANT -D_THREAD_SAFE -D_POSIX_PTHREAD_SEMANTICS

ifdef DO_RELDEB
	DO_RELEASE = 1
endif
ifdef DO_RELASSERT
	DO_RELEASE = 1
endif
ifdef DO_PROFILING
	DO_RELEASE = 1
endif
ifdef DO_RELEASE
	TARGET=release
	COMPILER_OPTIMIZATION_FLAGS = -O3 $(EXTRA_COMPILER_OPTIMIZATION_FLAGS)
	COMPILER_DEBUG_FLAGS = -DNDEBUG
	LIB_INFIX =
endif
ifdef DO_RELDEB
	TARGET=reldeb
	COMPILER_DEBUG_FLAGS = -g -DNDEBUG
	LIB_INFIX = -rd
	undefine DO_RELEASE # We want to keep program symbols.
endif
ifdef DO_RELASSERT
	TARGET=relassert
	COMPILER_DEBUG_FLAGS = -D__DEBUG__
	LIB_INFIX = -ra
endif
ifdef DO_DEBUG
	TARGET=debug
	COMPILER_DEBUG_FLAGS = -O0 -g -ggdb -g3 -ggdb3 \
												 -fno-inline -D__DEBUG__ \
												 $(EXTRA_COMPILER_DEBUG_FLAGS) $(DB)
	LIB_INFIX = -d
endif
ifdef DO_PROFILING
	TARGET=prof
	COMPILER_DEBUG_FLAGS = -g -DNDEBUG
	COMPILER_PROFILING_FLAGS = -pg
	LINKER_PROFILING_FLAGS   = -pg
	LIB_INFIX = -p
	undefine DO_RELEASE # We want to keep program symbols.
endif
ifdef DO_COVERAGE
	TARGET=cov
	COMPILER_COVERAGE_FLAGS = --coverage
	COMPILER_DEBUG_FLAGS = -D__DEBUG__ -fno-inline $(EXTRA_COMPILER_COVERAGE_FLAGS)
	LINKER_COVERAGE_FLAGS   = --coverage
	LIB_INFIX = -c
endif

# Solaris ld no longer supports --demangle option,
# we need to use ./configure to check this option is supported
# on host platform.
# LINKER_PRIME_FLAGS  = -Wl,--demangle

INCLUDE_PATHS = -I$(DIR_BUILD) $(sort -I$(DIR_ROOT) -I$(VPATH)) $(EXTRA_INCLUDE_PATHS)
LIBRARY_PATHS = -L$(DIR_BUILD) $(EXTRA_LIBRARY_PATHS)

override CXXFLAGS += $(strip $(CWARNING_FLAGS) $(CXXWARNING_FLAGS) \
		$(COMPILER_OPTIMIZATION_FLAGS) $(COMPILER_DEBUG_FLAGS) \
		$(COMPILER_PROFILING_FLAGS) $(COMPILER_COVERAGE_FLAGS) \
		$(INCLUDE_PATHS) $(CFLAGS) $(CXX_STANDARD) $(COMPILER_PRIME_FLAGS) \
		$(EXTRA_CXXFLAGS))

# WARNING!! $(LINKER_PRIME_FLAGS) must be last option,
# must be immediately followed by end of line
override LXXFLAGS = $(strip $(LIBRARY_PATHS) $(LINKER_PROFILING_FLAGS) \
		$(LINKER_COVERAGE_FLAGS) $(LDFLAGS) $(LINKER_PRIME_FLAGS) \
		$(EXTRA_LXXFLAGS))

# vim: ft=make
