tags: $(SRCS) $(HDRS)
	@printf "%b" "Rehashing tags ... " && \
	cd $(DIR_ROOT) && $(CTAGS) && $(GTAGS) &&  \
	printf "%b\n" "done."

DIR_COVERAGE=build/cov
LCOV_PARAMS=--rc lcov_excl_line='\b(M_EPILOG|M_DESTRUCTOR_EPILOG)\b'

prepare-coverage-baseline: $(DIR_COVERAGE)
	@cd $(DIR_ROOT) && \
	if [ ! -f "$(CLIENT_PATH)/Makefile.mk.in" ] ; then \
		echo "ERROR: Please specify library's client path with CLIENT_PATH variable." && \
		exit 1 ; \
	fi && \
	printf "%b" "Preparing coverage base line ... " && \
	find $(DIR_COVERAGE) -name '*.gcda' | xargs /bin/rm -f && \
	find $(CLIENT_PATH)/$(DIR_COVERAGE) -name '*.gcda' | xargs /bin/rm -f && \
	lcov $(LCOV_PARAMS) \
	     --base-directory $(DIR_ROOT) \
	     --directory $(DIR_COVERAGE)/hcore \
	     --directory $(DIR_COVERAGE)/tools \
	     --directory $(DIR_COVERAGE)/dbwrapper \
	     --directory $(DIR_COVERAGE)/hconsole \
	     --directory $(DIR_COVERAGE)/hdata \
	     --capture --initial --output-file $(DIR_COVERAGE)/$(LIB_NAME)-baseline.info && \
	lcov $(LCOV_PARAMS) \
	     --base-directory ${HOME}/usr/include \
	     --directory $(CLIENT_PATH)/$(DIR_COVERAGE)/$(lastword $(subst /, ,$(CLIENT_PATH))) \
	     --capture --initial --output-file $(DIR_COVERAGE)/$(LIB_NAME)-client-baseline.info && \
	printf "%b\n" "done."

coverage-stats:
	@cd $(DIR_ROOT) && \
	if [ ! -f "$(CLIENT_PATH)/Makefile.mk.in" ] ; then \
		echo "ERROR: Please specify library's client path with CLIENT_PATH variable." && \
		exit 1 ; \
	fi && \
	printf "%b" "Preparing coverage statistics ... " && \
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
	     --base-directory ${HOME}/usr/include \
	     --directory $(CLIENT_PATH)/$(DIR_COVERAGE)/$(lastword $(subst /, ,$(CLIENT_PATH))) \
	     --ignore-errors gcov,source \
	     --capture --output-file $(DIR_COVERAGE)/$(LIB_NAME)-client-test.info ;\
	sed -i -e 's@${HOME}/usr/include/$(LIB_NAME)@$(DIR_ROOT)@g' \
	     $(DIR_COVERAGE)/$(LIB_NAME)-client-baseline.info $(DIR_COVERAGE)/$(LIB_NAME)-client-test.info ;\
	lcov $(LCOV_PARAMS) \
	     --add-tracefile $(DIR_COVERAGE)/$(LIB_NAME)-baseline.info \
	     --add-tracefile $(DIR_COVERAGE)/$(LIB_NAME)-client-baseline.info \
	     --add-tracefile $(DIR_COVERAGE)/$(LIB_NAME)-test.info \
	     --add-tracefile $(DIR_ROOT)/$(DIR_COVERAGE)/$(LIB_NAME)-client-test.info \
	     --output-file $(DIR_ROOT)/$(DIR_COVERAGE)/$(LIB_NAME)-total.info && \
	lcov $(LCOV_PARAMS) \
	     --extract $(DIR_COVERAGE)/$(LIB_NAME)-total.info '*/$(LIB_NAME)/*' \
	     --output-file $(DIR_COVERAGE)/$(LIB_NAME)-coverage.info && \
	/bin/rm -rf $(DIR_ROOT)/build/coverage-stats && \
	$(MKDIR_P) $(DIR_ROOT)/build/coverage-stats && \
	genhtml $(LCOV_PARAMS) $(DIR_COVERAGE)/$(LIB_NAME)-coverage.info \
	        --legend --num-spaces=2 --output-directory=$(DIR_ROOT)/build/coverage-stats && \
	printf "%b\n" "done."

HASH=\#
CPPCHECK_CMD= cppcheck -D__ID__=\"\" -D__TID__=\"\" $(filter -D%,$(CXXFLAGS)) -DOPENSSL_THREADS=1 $(filter -I%,$(CXXFLAGS)) \
	$(shell awk '/$(HASH)undef /{printf "-U"$$3" "}' $(DIR_ROOT)/build/debug/config.hxx) \
	$(shell echo | $(CXX) -xc++ -E -v - 2>&1 | grep -v '[\t ]-' | awk '/^ \//{printf "-I"$$1" "}') \
	$(if $(SRC_TARGETS),$(if ${CPLUS_INCLUDE_PATH},-I$(subst :, -I,${CPLUS_INCLUDE_PATH}),),) \
	--enable=warning,style,performance,portability,information,missingInclude \
	--suppressions-list=$(DIR_ROOT)/_aux/cppcheck.supp \
	--inline-suppr --report-progress --verbose --error-exitcode=255 \
	$(if $(CPPCHECK_JOBS),-j$(CPPCHECK_JOBS),) \
	$(if $(CPPCHECK_TARGET),$(CPPCHECK_TARGET),$(if $(SRC_TARGETS),$(SRC_TARGETS),$(COMPONENTS))) 2>&1 | tee -a $(DIR_BUILD)/make.log

check: $(SRCS) $(HDRS)
	@cd $(DIR_ROOT) && if test -t 1 -a "x$${VERBOSE}" != "xyes" ; then \
		$(call invoke, $(CPPCHECK_CMD) | awk -v CL="`tput cr;tput dl1`" '{printf CL"%s\r", $$0}') ; \
	else \
		$(call invoke, $(CPPCHECK_CMD)) ; \
	fi

spell: $(SRCS) $(HDRS)
	@cd $(DIR_ROOT) && codespell -e UTF-8 -q 7 $(SRCS) $(HDRS)

