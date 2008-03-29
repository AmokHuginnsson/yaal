STATICLIBS = $(patsubst %, ~/usr/lib/lib%.a, $(LIBS))
STATICLIBS := $(subst lib-l%, %, $(STATICLIBS))
LIBS := $(patsubst %, -l%, $(LIBS))

STATICLIBS += -rdynamic -ldl -lssl -lxml2 -lpthread -lrt
STATICLIBS += -lncurses -lgpm
#STATICLIBS += -lccmalloc

