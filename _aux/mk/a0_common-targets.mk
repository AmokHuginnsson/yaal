#phony targets
.PHONY: all bin check clean clean-dep cov debug dep distclean doc install install-environment mrproper relassert reldeb release prof purge spell static stats uninstall

all: $(TARGET)

debug relassert reldeb release prof cov: environment $(REAL_TARGETS)

environment: $(DIR_BUILD) $(DIRS)

$(DIR_BUILD):
	@$(MKDIR_P) $(@)

$(PRJNAME).info : src/$(PRJNAME).texinfo
	@makeinfo src/$(PRJNAME).texinfo

static:
	@$(MAKE) LIBS="$(STATICLIBS)"

clean-dep:
	@$(FIND) . -name '*.$(DS)' | xargs /bin/rm -f

clean: clean-dep
	@/bin/rm -f *~ *.out .*~ .*.swp *.bak *.log *.info .depend
	@/bin/rm -f core $(TARGET).core 1exec.core journal.tut log.tut
	@/bin/rm -f src/core src/$(TARGET).core src/1exec.core
	@/bin/rm -f $(OBJS_TARGET)
	@/bin/rm -f $(DIR_BUILD)/$(TARGET)/$(EXEC_NAME)
	@sh -c '. $(DIR_ROOT)/_aux/clean-lib.sh && clean .'

mrproper: clean
	@printf "%b" "Purging ... "; \
	/bin/rm -f version.hxx src/.gt_* src/tags 1exec.core; \
	/bin/rm -rf src/1exec.core $(PRJNAME); \
	$(FIND) . \( -name .git -prune -name 'tags' -o -name '.depend' -o -name '*.a' \) -a ! -name .git \
| xargs /bin/rm -f; \
	cd .. && /bin/rm -rf $(DIR_BUILD); \
	printf "%b\n" "done."

bin:
	@( DO_RELEASE=1 $(MAKE) ; make clean )

install: all
	@$(call msg,printf "%b\n" "i: you need to become root to do this" && ) \
	$(call invoke,$(INSTALL) -d -m755 $(DIR_BIN) && $(INSTALL) -m755 $(DIR_BUILD)/$(PRJNAME)/$(EXEC_NAME) $(DIR_BIN)/$(PRJNAME) && ) \
	$(call invoke,$(INSTALL) -d -m755 $(DIR_SYSCONF) && $(INSTALL) -m644 $(DIR_ROOT)/$(PRJNAME)rc $(DIR_SYSCONF)/ && ) \
	$(call invoke,$(INSTALL) -d -m755 $(DIR_DOC) && $(INSTALL) -m644 $(DIR_ROOT)/doc/README $(DIR_ROOT)/doc/COPYRIGHT $(DIR_DOC)/ && ) \
	$(call msg,printf "%b\n" "i: now if you wish you can suid $(DIR_BIN)/$(PRJNAME)" && ) \
	$(call msg,printf "%b$(NL)" "done.$(CL)")

