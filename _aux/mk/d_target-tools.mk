tags: $(SRCS) $(HDRS)
	@printf "%b" "Rehashing tags ... "; \
	cd $(DIR_ROOT) && $(CTAGS) && \
	printf "%b\n" "done."

check: $(SRCS) $(HDRS)
	@cd $(DIR_ROOT) && cppcheck -D__ID__=\"\" -D__TID__=\"\" $(filter -D%,$(CXXFLAGS)) $(filter -I%,$(CXXFLAGS)) --enable=all --suppressions-list=$(DIR_ROOT)/_aux/cppcheck-suppressions.list --report-progress --verbose $(if $(CPPCHECK_JOBS),-j$(CPPCHECK_JOBS),) $(COMPONENTS) 2>&1 | tee -a $(DIR_BUILD)/make.log

