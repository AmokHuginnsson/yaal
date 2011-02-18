EID=$(shell id -u)
ifeq ($(EID),0)
$(error running with super-user privileges - bailing out)
endif

