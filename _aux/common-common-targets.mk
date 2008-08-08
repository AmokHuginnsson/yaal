#phony targets
.PHONY: all dep static prof clean clean-dep tags mrproper purge bin install environment

ifneq ($(CURDIR),$(DIR_BUILD))
all: environment
	@test -t 1 && export TERMINAL="TERM" ; \
	$(MAKE) --no-print-directory -C $(DIR_BUILD) -f ../Makefile
else
all: $(REAL_TARGETS)
endif

environment: $(DIR_BUILD) $(DIRS)

$(DIR_BUILD):
	@/bin/mkdir -p $(@); \
	echo "Program version $(RELEASE)-`date +%Y%m%d`."; \
	echo "#define VER \"$(RELEASE)-`date +%Y%m%d`\"" > $(DIR_BUILD)/version.h

$(PRJNAME).info : src/$(PRJNAME).texinfo
	@makeinfo src/$(PRJNAME).texinfo

static:
	@$(MAKE) LIBS="$(STATICLIBS)"

prof:
	@DO_PROFILING=1 $(MAKE) static

cov:
	@DO_COVERAGE=1 $(MAKE) static

clean-dep:
	@$(FIND) . -name '*.$(DS)' | xargs /bin/rm -f

clean: clean-dep
	@/bin/rm -f *~ *.out .*~ .*.swp *.bak *.log *.info .depend
	@/bin/rm -f core $(TARGET).core 1exec.core journal.tut log.tut
	@/bin/rm -f src/core src/$(TARGET).core src/1exec.core
	@/bin/rm -f $(OBJS_TARGET)
	@/bin/rm -f $(DIR_BUILD)/$(TARGET)/$(EXEC_NAME)
	@cln .

mrproper: clean
	@echo -n "Purging ... "; \
	/bin/rm -f $(PRJNAME) version.h src/.gt_* src/tags 1exec.core; \
	/bin/rm -rf src/1exec.core $(DIR_BUILD); \
	$(FIND) . \( -name .git -prune -name 'tags' -or -name '.depend' -or -name '*.a' \) -a ! -name .git \
| xargs /bin/rm -f; \
	echo "done."

purge: mrproper
	/bin/rm -rf autom4te.cache config.cache config.status configure.lineno; \
	/bin/rm -rf configure Makefile config.h config.h.in config.log

bin:
	@( NODEBUG="true" $(MAKE) ; make clean )

install: $(TARGET)
	@echo "i: you need to become root to do this"
	@install -c -g root -o root -m=u=rwx,g=rx,o=rx $(PRJNAME) $(prefix)
#	@install -D -g root -o root -m=u=rwx,g=rx,o=rx README COPYRIGHT $(doc)/$(PRJNAME)
#	@install -D -g root -o root -m=u=rwx,g=rx,o=rx .$(PRJNAME)rc $(doc)/$(PRJNAME)/examples
	@echo "i: now if you wish you can suid $(TARGET)"

