CWARNING_FLAGS =	\
						-Wall \
						-Wcast-align \
						-Wcast-qual \
						-Wconversion \
						-Wendif-labels \
						-Werror \
						-Winline \
						-Wlarger-than-16384 \
						-Wlong-long \
						-Wmissing-noreturn \
						-Wpointer-arith \
						-Wredundant-decls \
						-Wsign-compare \
						-Wundef \
						-Wwrite-strings
#						-Wbad-function-cast \
						-Wmissing-declarations \
						-Wmissing-prototypes \
						-Wnested-externs \
						-Wstrict-prototypes \
						-Wtraditional \ #C only
#						-Wmissing-format-attribute \
						-Wunreachable-code \ # too many warrnings
#						-Waggregate-return \ # useless for C++
CXXWARNING_FLAGS		=	\
						-Weffc++ \
						-Wsign-promo
#						-Wold-style-cast \ # lots of warrnings from libc headers
#						-Woverloaded-virtual \ # too many unecessary warnings
ifeq ($(HD),1)
	DB = -D__DEBUGGER_BABUNI__
endif
COMPILER_PRIME_FLAGS					= -fmessage-length=0 -std=gnu++98 -pipe \
																-D_GNU_SOURCE

ifdef DO_RELDEB
	DO_RELEASE = 1
endif
ifdef DO_RELASSERT
	DO_RELEASE = 1
endif
ifdef DO_RELEASE
	TARGET=release
	COMPILER_OPTIMIZATION_FLAGS = -O3 -fexpensive-optimizations -Wno-error
	COMPILER_DEBUG_FLAGS = -DNDEBUG
	LIB_INFIX =
endif
ifdef DO_RELDEB
	TARGET=reldeb
	COMPILER_DEBUG_FLAGS = -g
	LIB_INFIX = -rd
endif
ifdef DO_RELASSERT
	TARGET=relassert
	COMPILER_DEBUG_FLAGS = -D__DEBUG__
	LIB_INFIX = -ra
endif
ifdef DO_DEBUG
	TARGET=debug
	COMPILER_DEBUG_FLAGS = -O0 -g3 -ggdb3 -fno-inline -D__DEBUG__ $(DB)
	LIB_INFIX = -d
endif
ifdef DO_PROFILING
	TARGET=prof
	COMPILER_PROFILING_FLAGS = -pg
	COMPILER_DEBUG_FLAGS = -D__DEBUG__
	LINKER_PROFILING_FLAGS   = -pg
	LIB_INFIX = -p
endif
ifdef DO_COVERAGE
	TARGET=cov
	COMPILER_COVERAGE_FLAGS = --coverage
	COMPILER_DEBUG_FLAGS = -D__DEBUG__
	LINKER_COVERAGE_FLAGS   = --coverage
	LIB_INFIX = -c
endif
LINKER_PRIME_FLAGS  = -Wl,--demangle

COMPILER_PATH_FLAGS = -I$(DIR_BUILD) -I$(DIR_ROOT) -I$(VPATH) -I/usr/local/include
LINKER_PATH_FLAGS   = -L/usr/local/lib

CXXFLAGS += $(CWARNING_FLAGS) $(CXXWARNING_FLAGS) \
						$(COMPILER_OPTIMIZATION_FLAGS) $(COMPILER_DEBUG_FLAGS) \
						$(COMPILER_PROFILING_FLAGS) $(COMPILER_COVERAGE_FLAGS) \
						$(COMPILER_PATH_FLAGS) $(CFLAGS) $(COMPILER_PRIME_FLAGS)

# WARNING!! $(LINKER_PRIME_FLAGS) must be last option,
# must be immediately followed by end of line
LXXFLAGS	= $(LINKER_PATH_FLAGS) $(LINKER_PROFILING_FLAGS) \
						$(LINKER_COVERAGE_FLAGS) $(LDFLAGS) $(LINKER_PRIME_FLAGS)

# vim: ft=make
