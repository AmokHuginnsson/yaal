SHELL     = /bin/sh
RELEASE   = $(VERSION).$(SUBVERSION).$(EXTRAVERSION)
DIR_PKGCONFIG= $(DIR_SHARE)/pkgconfig
DIR_BUILD ?= $(CURDIR)
FIND      = find
ARFLAGS   = cr
VPATH     = $(DIR_ROOT)
ifeq ($(LIB_PREFIX),lib)
LIB_SUFFIX   = so.$(RELEASE)
LIB_ARCHIVE_SUFFIX = a
else
LIB_SUFFIX   = dll
LIB_ARCHIVE_SUFFIX = dll.a
endif

define IDENT_HELPER
	DIR_PREFIX=$$(subst . /,./,$$(foreach IT,$$(subst /, ,$$(subst $$(DIR_ROOT),,$$(DIR_BUILD))),/..))
	HEADER=$$(addsuffix .hxx,$$(basename $(1)))
	ID="\"$(1) $$(shell $$(GITID) ./$$(DIR_PREFIX)/$$(subst $$(DIR_ROOT)/,,$(1)))\""
	TID="\"$$(HEADER) $$(if $$(wildcard $$(HEADER)),$$(shell $$(GITID) ./$$(DIR_PREFIX)/$$(subst $$(DIR_ROOT)/,,$$(HEADER))),)\""
endef

# this neat trick does not really blong here
# but we want client makefiles look really simple

define NEW_TARGET
	TARGET_$(1) := $(1)
	SRC_TARGET_$(1) := $(2)
	COMPILER_FLAGS_TARGET_$(1) := $(3)
	LXXFLAGS_TARGET_$(1) := $(4)
	LIBS_TARGET_$(1) := $(5)
	EXTRA_DEPS_TARGET_$(1) := $(6)
	TARGETS := $$(TARGETS) TARGET_$(1)
	SRC_TARGETS := $$(SRC_TARGETS) $(2)
endef

,        :=,
SPACE    :=
SPACE    +=
$(SPACE) :=
$(SPACE) +=

uniq = $(if $(1),$(firstword $(1)) $(call uniq,$(filter-out $(firstword $(1)),$(1))))
quotecomma=$(subst $(,),$$(,),$(subst -EOC,,$(subst $(,)-EOC,,$(subst $(,)$(,),,$(1),$(2),$(3),$(4),$(5),$(6),$(7),$(8),$(9),$(10),$(11),$(12),$(13),$(14),$(15),-EOC))))
TO_LOWER = $(subst A,a,$(subst B,b,$(subst C,c,$(subst D,d,$(subst E,e,$(subst F,f,$(subst G,g,$(subst H,h,$(subst I,i,$(subst J,j,$(subst K,k,$(subst L,l,$(subst M,m,$(subst N,n,$(subst O,o,$(subst P,p,$(subst Q,q,$(subst R,r,$(subst S,s,$(subst T,t,$(subst U,u,$(subst V,v,$(subst W,w,$(subst X,x,$(subst Y,y,$(subst Z,z,$(1)))))))))))))))))))))))))))
TO_UPPER = $(subst a,A,$(subst b,B,$(subst c,C,$(subst d,D,$(subst e,E,$(subst f,F,$(subst g,G,$(subst h,H,$(subst i,I,$(subst j,J,$(subst k,K,$(subst l,L,$(subst m,M,$(subst n,N,$(subst o,O,$(subst p,P,$(subst q,Q,$(subst r,R,$(subst s,S,$(subst t,T,$(subst u,U,$(subst v,V,$(subst w,W,$(subst x,X,$(subst y,Y,$(subst z,Z,$(1)))))))))))))))))))))))))))
first = $(firstword $(subst :, ,$(1)))
second = $(word 2,$(subst :, ,$(1)))

