#phony targets
.PHONY: all bin clean clean-dep cov debug dep doc install install-environment mrproper release prof purge static stats tags 

all: debug

debug: environment $(REAL_TARGETS)

environment: $(DIR_BUILD) $(DIRS)

$(DIR_BUILD):
	@/bin/mkdir -p $(@)

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
	@echo -n "Purging ... "; \
	/bin/rm -f version.hxx src/.gt_* src/tags 1exec.core; \
	/bin/rm -rf src/1exec.core $(PRJNAME) $(DIR_BUILD); \
	$(FIND) . \( -name .git -prune -name 'tags' -or -name '.depend' -or -name '*.a' \) -a ! -name .git \
| xargs /bin/rm -f; \
	echo "done."

purge: mrproper
	/bin/rm -rf autom4te.cache config.cache config.status configure.lineno; \
	/bin/rm -rf configure Makefile.mk config.hxx config.hxx.in config.log

bin:
	@( DO_RELEASE=1 $(MAKE) ; make clean )

install: $(TARGET)
	@echo "i: you need to become root to do this"
	@install -c -g root -o root -m=u=rwx,g=rx,o=rx $(PRJNAME) $(prefix)
#	@install -D -g root -o root -m=u=rwx,g=rx,o=rx README COPYRIGHT $(doc)/$(PRJNAME)
#	@install -D -g root -o root -m=u=rwx,g=rx,o=rx .$(PRJNAME)rc $(doc)/$(PRJNAME)/examples
	@echo "i: now if you wish you can suid $(TARGET)"

