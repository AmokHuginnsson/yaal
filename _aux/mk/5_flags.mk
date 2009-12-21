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

GROUP_START = -Wl,-\(
GROUP_END = -Wl,-\)

# vim: ft=make
