CWARNING_FLAGS =	\
						-Wall\
						-Wcast-align\
						-Wcast-qual\
						-Wconversion\
						-Wendif-labels\
						-Werror\
						-Winline\
						-Wlarger-than-4096\
						-Wlong-long\
						-Wmissing-noreturn\
						-Wpointer-arith\
						-Wredundant-decls\
						-Wsign-compare\
						-Wundef\
						-Wwrite-strings
#						-Wbad-function-cast\
						-Wmissing-declarations\
						-Wmissing-prototypes\
						-Wnested-externs\
						-Wstrict-prototypes\
						-Wtraditional\ #C only
#						-Wmissing-format-attribute\
						-Wunreachable-code\ # too much warrnings
#						-Waggregate-return\ # useless for C++
CXXWARNING_FLAGS		=	\
						-Weffc++\
						-Wsign-promo
#						-Wold-style-cast\ # lots of warrnings from libc headers
#						-Woverloaded-virtual\ # too much unecessary warrnings
ifeq ($(HD),1)
	DB = -D__DEBUGGER_BABUNI__
endif
COMPILER_PRIME_FLAGS					= -fmessage-length=0 -std=gnu++98 -pipe \
																-D_GNU_SOURCE
ifdef DO_RELEASE
	COMPILER_OPTIMIZATION_FLAGS = -O3 -fexpensive-optimizations -DNDEBUG -Wno-error
else
	COMPILER_DEBUG_FLAGS				= -g3 -ggdb3 -fno-inline -D__DEBUG__ $(DB)
endif
ifdef DO_PROFILING
	COMPILER_PROFILING_FLAGS = -pg
	LINKER_PROFILING_FLAGS = -pg
endif
ifdef DO_COVERAGE
	COMPILER_COVERAGE_FLAGS = --coverage
	LINKER_COVERAGE_FLAGS = --coverage
endif
LINKER_PRIME_FLAGS						= \
								-Wl,--demangle

# vim: ft=make
COMPILER_PATH_FLAGS						= -I$(DIR_ROOT) -I$(DIR_BUILD) -I$(VPATH) \
																-I/usr/local/include
LINKER_PATH_FLAGS							= -L. -L$(DIR_TARGET) -L/usr/local/lib

CXXFLAGS += $(CWARNING_FLAGS) $(CXXWARNING_FLAGS) \
						$(COMPILER_OPTIMIZATION_FLAGS) $(COMPILER_DEBUG_FLAGS) \
						$(COMPILER_PROFILING_FLAGS) $(COMPILER_COVERAGE_FLAGS) \
						$(COMPILER_PATH_FLAGS) $(CFLAGS) $(COMPILER_PRIME_FLAGS)

# WARNING!! $(LINKER_PRIME_FLAGS) must be last option,
# must be immediately followed by end of line
LXXFLAGS	= $(LINKER_PATH_FLAGS) $(LINKER_PROFILING_FLAGS) \
						$(LINKER_COVERAGE_FLAGS) $(LDFLAGS) $(LINKER_PRIME_FLAGS)
