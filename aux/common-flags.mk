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
																-D_GNU_SOURCE -march=i486
ifdef NODEBUG
	COMPILER_OPTIMIZATION_FLAGS = -O3 -fexpensive-optimizations
else
	COMPILER_DEBUG_FLAGS				= -g -g3 -ggdb -ggdb3 -pg -fno-inline \
																-D__DEBUG__ $(DB)
endif
LINKER_PRIME_FLAGS						= \
								-Wl,--fatal-warnings \
								-Wl,--demangle
