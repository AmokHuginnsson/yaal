ifeq ($(CURDIR),$(DIR_BUILD))
HDRS = $(strip $(sort $(shell cd $(DIR_ROOT)/src && $(FIND) . -name "*.$(HS)")))
SRCS = $(strip $(sort $(shell cd $(DIR_ROOT)/src && $(FIND) . -name "*.$(SS)")))
OBJS = $(patsubst %.$(SS),%.$(OS),$(SRCS))
endif

