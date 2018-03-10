ifdef STATIC

STATICLIBS = -Wl,--whole-archive
STATICLIBS += $(patsubst %,%.a,$(patsubst -l%,$(prefix)/lib/lib%,$(filter -lyaal_%,$(call uniq,$(EXTRA_LIBS)))))
STATICLIBS += -Wl,--no-whole-archive
SHAREDLIBS = $(filter-out -lyaal_%,$(EXTRA_LIBS))
STATICLIBS += $(SHAREDLIBS) -rdynamic -ldl -lssl -lcrypto -lpthread -lpcre -lrt
STATICLIBS += $(if $(filter -lyaal_tools$(LIB_INFIX),$(EXTRA_LIBS)),-lz -lxslt -lexslt -lxml2)
STATICLIBS += $(if $(filter -lyaal_hconsole$(LIB_INFIX),$(EXTRA_LIBS)),-lncurses -lgpm)
STATIC_SUFFIX=-static

endif

LIBS := $(subst -l-l,-l,$(patsubst %, -l%, $(LIBS)))

