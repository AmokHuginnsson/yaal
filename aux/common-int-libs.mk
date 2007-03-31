STATICLIBS = $(patsubst %, ~/lib/lib%.a, $(LIBS))
LIBS := $(patsubst %, -l%, $(LIBS))

STATICLIBS += -rdynamic -ldl
STATICLIBS += -lncurses
#STATICLIBS += -lccmalloc

