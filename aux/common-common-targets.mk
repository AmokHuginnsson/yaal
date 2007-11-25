#phony targets
.PHONY: all dep static clean tags mrproper purge bin install

ifneq ($(CURDIR),$(DIR_BUILD))
all: $(DIR_BUILD)
	@test -t 1 && export TERMINAL="TERM" ; \
	$(MAKE) --no-print-directory -C $(DIR_BUILD) -f ../Makefile
else
all: $(TARGET)
endif

$(DIR_BUILD):
	@/bin/mkdir -p $(@); \
	echo "Program version $(RELEASE)-`date +%Y%m%d`."; \
	echo "#define VER \"$(RELEASE)-`date +%Y%m%d`\"" > $(DIR_BUILD)/version.h

header.gch: $(VPATH)/header $(HDRS)
	@echo -n "Generating precompiled header \`$(subst $(DIR_ROOT)/,,$(<))' ... "; \
	$(CXX) -xc++-header $(subst -include header,,$(CXXFLAGS)) $(<) -o $(@) ;\
	echo $(NONL) "done.$(CL)"

$(TARGET): header.gch $(OBJS)
	@echo -n "Linking \`$@' ... "
	@$(LXX) $(LINKER_FLAGS) -o $(TARGET) $(OBJS) $(LIBS) 2>&1 | tee -a make.log
ifdef NODEBUG
	@strip $(TARGET)
endif
	@echo "done."

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
	@/bin/rm -f $(OBJS)
	@/bin/rm -f $(TARGET)
	@cln .

mrproper: clean
	@echo -n "Purging ... "; \
	/bin/rm -f $(PRJNAME) version.h src/.gt_* src/tags 1exec.core; \
	/bin/rm -rf src/1exec.core $(DIR_BUILD); \
	$(FIND) . -name 'tags' -or -name '.depend' -or -name '*.a' \
| xargs /bin/rm -f; \
	echo "done."

purge: mrproper
	/bin/rm -rf autom4te.cache config.cache config.status configure.lineno; \
	/bin/rm -rf configure Makefile config.h config.log

bin:
	@( NODEBUG="true" $(MAKE) ; make clean )

install: $(TARGET)
	@echo "i: you need to become root to do this"
	@install -c -g root -o root -m=u=rwx,g=rx,o=rx $(PRJNAME) $(prefix)
#	@install -D -g root -o root -m=u=rwx,g=rx,o=rx README COPYRIGHT $(doc)/$(PRJNAME)
#	@install -D -g root -o root -m=u=rwx,g=rx,o=rx .$(PRJNAME)rc $(doc)/$(PRJNAME)/examples
	@echo "i: now if you wish you can suid $(TARGET)"

