ifeq ($(TERMINAL),TERM)
CL						:= $(shell tput cr;tput dl1)
BOLD					:= $(shell tput bold || tput md)
RED						:= $(shell tput setaf 1 || tput AF 1)
RS						:= $(shell tput sgr0 || tput me)
NONL					:= "-n"
endif
