EID=$(shell id -u)
ifeq ($(EID),0)
$(error running with super-user privileges - bailing out)
endif
UMASK=$(subst 0077,77,$(shell umask))
ifneq ($(UMASK),77)
$(error running with too permissive umask: `$(UMASK)', must be: `0077' - bailing out)
endif

.DELETE_ON_ERROR:

