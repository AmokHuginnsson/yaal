tags: $(SRCS) $(HDRS)
	@printf "%b" "Rehashing tags ... "; \
	cd $(DIR_ROOT) && $(CTAGS) && \
	printf "%b\n" "done."

CPPCHECK_CMD= cppcheck -D__ID__=\"\" -D__TID__=\"\" $(filter -D%,$(CXXFLAGS)) -DOPENSSL_THREADS=1 $(filter -I%,$(CXXFLAGS)) \
	$(if $(SRC_TARGETS),$(if ${CPLUS_INCLUDE_PATH},-I$(subst :, -I,${CPLUS_INCLUDE_PATH}),),) \
	--enable=all --inline-suppr --report-progress --verbose \
	$(if $(CPPCHECK_JOBS),-j$(CPPCHECK_JOBS),) \
	$(if $(CPPCHECK_TARGET),$(CPPCHECK_TARGET),$(if $(SRC_TARGETS),$(SRC_TARGETS),$(COMPONENTS))) 2>&1 | tee -a $(DIR_BUILD)/make.log

check: $(SRCS) $(HDRS)
	@cd $(DIR_ROOT) && if test -t 1 -a "x$${VERBOSE}" != "xyes" ; then \
		$(CPPCHECK_CMD) | awk -v CL="`tput cr;tput dl1`" '{printf CL"%s\r", $$0}' ; \
	else \
		$(CPPCHECK_CMD) ; \
	fi

