ifeq ($(CURDIR),$(DIR_BUILD))
ifeq ($(strip $(foreach GOAL,clean mrproper purge-local stats tags doc,$(findstring $(GOAL),$(MAKECMDGOALS)))),)
$(shell $(DIR_ROOT)/_aux/update-commit-id $(DIR_ROOT) $(DIR_BUILD)/commit_id.hxx)
endif
endif
