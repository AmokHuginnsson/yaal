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
	@printf "%b" "Purging ... "; \
	/bin/rm -f version.hxx src/.gt_* src/tags 1exec.core; \
	/bin/rm -rf src/1exec.core $(PRJNAME) $(DIR_BUILD); \
	$(FIND) . \( -name .git -prune -name 'tags' -or -name '.depend' -or -name '*.a' \) -a ! -name .git \
| xargs /bin/rm -f; \
	printf "%b\n" "done."

purge: mrproper
	if [ -d _aux -a ! -h _aux ] ; then /bin/rm -f _aux/config.guess _aux/config.sub _aux/install-sh _aux/ltmain.sh _aux/missing ; fi && \
	/bin/rm -rf aclocal.m4 autom4te.cache build config.cache config.status \
		configure.lineno configure.scan configure Makefile.mk config.hxx config.hxx.in \
		config.h config.h.in yaalrc config.log dirs.d doc/html \
		CMakeFiles CMakeCache.txt cmake_install.cmake \
		tags GPATH GRTAGS GSYMS GTAGS make.log *.vcproj.* *.vcproj \
		yaal.sln yaal.suo yaal.ncb *.dir debug release *.so && \
	if [ "x${OSTYPE}" != "xcygwin" ] ; then /bin/rm -f Makefile ; fi

bin:
	@( DO_RELEASE=1 $(MAKE) ; make clean )

install: $(TARGET)
	@printf "%b\n" "i: you need to become root to do this"
	@install -c -g root -o root -m=u=rwx,g=rx,o=rx $(PRJNAME) $(prefix)
#	@install -D -g root -o root -m=u=rwx,g=rx,o=rx README COPYRIGHT $(doc)/$(PRJNAME)
#	@install -D -g root -o root -m=u=rwx,g=rx,o=rx .$(PRJNAME)rc $(doc)/$(PRJNAME)/examples
	@printf "%b\n" "i: now if you wish you can suid $(TARGET)"

