tags: $(SRCS) $(HDRS)
	@printf "%b" "Rehashing tags ... "; \
	cd $(DIR_ROOT) && $(CTAGS) && \
	printf "%b\n" "done."

DIR_COVERAGE=build/cov
LCOV_PARAMS=--rc lcov_branch_coverage=1

prepare-coverage-baseline: $(DIR_COVERAGE)
	@printf "%b" "Preparing coverage base line ... "; \
	cd $(DIR_ROOT) && \
	find $(DIR_COVERAGE) -name '*.gcda' | xargs /bin/rm -f && \
	lcov $(LCOV_PARAMS) \
	     --base-directory $(DIR_ROOT) \
	     --directory $(DIR_COVERAGE)/hcore \
	     --directory $(DIR_COVERAGE)/tools \
	     --directory $(DIR_COVERAGE)/dbwrapper \
	     --directory $(DIR_COVERAGE)/hconsole \
	     --directory $(DIR_COVERAGE)/hdata \
	     --capture --initial --output-file $(DIR_COVERAGE)/$(LIB_NAME)-baseline.info && \
	printf "%b\n" "done."

coverage-stats:
	@printf "%b" "Preparing coverage statistics ... "; \
	cd $(DIR_ROOT) && \
	lcov $(LCOV_PARAMS) \
	     --base-directory $(DIR_ROOT) \
	     --directory $(DIR_COVERAGE)/hcore \
	     --directory $(DIR_COVERAGE)/tools \
	     --directory $(DIR_COVERAGE)/dbwrapper \
	     --directory $(DIR_COVERAGE)/hconsole \
	     --directory $(DIR_COVERAGE)/hdata \
			 --ignore-errors gcov,source \
	     --capture --output-file $(DIR_COVERAGE)/$(LIB_NAME)-test.info ;\
	lcov $(LCOV_PARAMS) \
	     --add-tracefile $(DIR_COVERAGE)/$(LIB_NAME)-baseline.info \
			 --add-tracefile $(DIR_COVERAGE)/$(LIB_NAME)-test.info \
			 --output-file $(DIR_COVERAGE)/$(LIB_NAME)-total.info && \
	lcov $(LCOV_PARAMS) \
	     --remove $(DIR_COVERAGE)/$(LIB_NAME)-total.info '/usr/include/*' '/usr/lib/*' \
			 --output-file $(DIR_COVERAGE)/$(LIB_NAME)-coverage.info && \
	/bin/rm -rf $(DIR_ROOT)/build/coverage-stats && \
	$(MKDIR_P) $(DIR_ROOT)/build/coverage-stats && \
	genhtml $(LCOV_PARAMS) $(DIR_COVERAGE)/$(LIB_NAME)-coverage.info --legend --output-directory=$(DIR_ROOT)/build/coverage-stats && \
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

