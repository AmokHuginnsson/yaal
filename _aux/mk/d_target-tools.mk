tags: $(SRCS) $(HDRS)
	@printf "%b" "Rehashing tags ... "; \
	cd $(DIR_ROOT) && $(CTAGS) && \
	printf "%b\n" "done."

check: $(SRCS) $(HDRS)
	@cd $(DIR_ROOT) && $(call invoke,cppcheck -D__ID__=\"\" -D__TID__=\"\" $(filter -D%,$(CXXFLAGS)) -DOPENSSL_THREADS=1 $(filter -I%,$(CXXFLAGS)) \
		$(if $(SRC_TARGETS),$(if ${CPLUS_INCLUDE_PATH},-I$(subst :, -I,${CPLUS_INCLUDE_PATH}),),) \
		--enable=all --inline-suppr --report-progress --verbose \
		$(if $(CPPCHECK_JOBS),-j$(CPPCHECK_JOBS),) $(if $(CPPCHECK_TARGET),$(CPPCHECK_TARGET),$(if $(SRC_TARGETS),$(SRC_TARGETS),$(COMPONENTS))) 2>&1 | tee -a $(DIR_BUILD)/make.log)

