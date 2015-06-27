STATICLIBS := $(foreach SL,$(patsubst %,\%%.a,$(LIBS)),$(filter $(SL),$(wildcard ~/usr/lib/lib*.a)))
SHAREDLIBS := $(patsubst %, -l%,$(filter-out $(patsubst ${HOME}/usr/lib/lib%.a,%,$(STATICLIBS)),$(LIBS)))
LIBS := $(subst -l-l,-l,$(patsubst %, -l%, $(LIBS)))

STATICLIBS += -rdynamic -ldl -lssl -lxslt -lxml2 -lpthread -lrt
STATICLIBS += -lncurses -lgpm $(SHAREDLIBS)
#STATICLIBS += -lccmalloc

