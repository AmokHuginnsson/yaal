tags: $(SRCS) $(HDRS)
	@printf "%b" "Rehashing tags ... "; \
	cd $(DIR_ROOT) && $(CTAGS) && \
	printf "%b\n" "done."

check: $(SRCS) $(HDRS)
	@cd $(DIR_ROOT) && cppcheck -D__ID__=\"\" -D__TID__=\"\" $(filter -D%,$(CXXFLAGS)) $(filter -I%,$(CXXFLAGS)) --enable=all --inline-suppr --report-progress --verbose $(if $(CPPCHECK_JOBS),-j$(CPPCHECK_JOBS),) $(if $(CPPCHECK_TARGET),$(CPPCHECK_TARGET),$(COMPONENTS)) 2>&1 | tee -a $(DIR_BUILD)/make.log

