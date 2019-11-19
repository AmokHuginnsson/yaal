#phony targets
.PHONY: all bin check clean clean-dep cov debug dep distclean doc install install-environment install-local mrproper relassert reldeb release prof purge spell stats uninstall uninstall-local

$(foreach IT,$(TARGETS),$(eval $(call PREPARE_TARGET,$(IT))))
$(foreach IT,$(TARGETS),$(eval $(call BUILD_TARGET,$(IT))))
$(foreach IT,$(DIRS),$(eval $(call MAKE_DIR,$(IT))))

all: $(TARGET)

debug relassert reldeb release prof cov: environment $(REAL_TARGETS)

environment: $(DIR_BUILD) $(DIRS)

$(DIR_BUILD):
	@$(MKDIR_P) $(@)

$(PRJNAME).info : src/$(PRJNAME).texinfo
	@makeinfo src/$(PRJNAME).texinfo

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

doc: $(DIR_ROOT)/build/doc/$(PRJNAME).1

$(DIR_ROOT)/build/doc/$(PRJNAME).1: $(DIR_ROOT)/build/doc/$(PRJNAME).1.txt
	env -u PREFIX asciidoctor -b manpage $(<) && \
	perl -i -0pe 's/.RE\n.sp\n.RS/.RE\n.RS/g' $(@)

$(DIR_ROOT)/build/doc/$(PRJNAME).1.txt: $(DIR_ROOT)/build/release/$(PRJNAME)/1exec
	cd $(DIR_ROOT) && mkdir -p $(DIR_ROOT)/build/doc && \
	$(DIR_ROOT)/_aux/help-to-asciidoc -p $(PRJNAME) -o $(@)

do-install: all
	@$(call msg_always,printf "%b\n" "i: you need to become root to do this" && ) \
	$(if $(CLOU_BIN), $(call invoke,$(INSTALL) -d -m755 $(DIR_BIN) $(foreach clou,$(CLOU_BIN),&& $(INSTALL) -m755 $(clou) $(DIR_BIN)/$(patsubst %$(EXEC_NAME),$(PRJNAME),$(clou)) && ) ) ) \
	$(if $(CLOU_LIB), $(call invoke,$(INSTALL) -d -m755 $(DIR_LIB) && $(INSTALL) -m755 $(CLOU_LIB) $(DIR_LIB)/ && ) ) \
	$(if $(CLOU_MODULES), $(call invoke,$(INSTALL) -d -m755 $(DIR_LIB)/$(PRJNAME) && $(INSTALL) -m644 $(CLOU_MODULES) $(DIR_LIB)/$(PRJNAME)/ && ) ) \
	$(if $(CLOU_CONF), \
		$(call invoke,$(INSTALL) -d -m755 $(dir $(DIR_SYSCONF)/$(call second,$(firstword $(CLOU_CONF)))) && \
		$(foreach clou,$(CLOU_CONF),$(INSTALL) -m644 $(call first,$(clou)) $(DIR_SYSCONF)/$(call second,$(clou)) && ) ) \
	) \
	$(if $(CLOU_SHARE), $(call invoke,$(INSTALL) -d -m755 $(DIR_SHARE)/$(PRJNAME) && $(INSTALL) -m644 $(CLOU_SHARE) $(DIR_SHARE)/$(PRJNAME)/ && ) ) \
	$(if $(CLOU_LOCALSTATE), $(call invoke,$(INSTALL) -d -m755 $(DIR_LOCALSTATE)/$(PRJNAME) && $(INSTALL) -m644 $(CLOU_LOCALSTATE) $(DIR_LOCALSTATE)/$(PRJNAME)/ && ) ) \
	$(if $(CLOU_INCLUDE), $(call invoke,$(INSTALL) -d -m755 $(DIR_HEADERS) && $(INSTALL) -m644 $(CLOU_INCLUDE) $(DIR_HEADERS)/ && ) ) \
	$(if $(CLOU_DOC), $(call invoke,$(INSTALL) -d -m755 $(DIR_DOC) && $(INSTALL) -m644 $(CLOU_DOC) $(DIR_DOC)/ && ) ) \
	$(if $(CLOU_BIN), $(call invoke,$(foreach clou,$(CLOU_MAN),$(INSTALL) -d -m755 $(DIR_MAN)/man$(subst .,,$(suffix $(clou))) && $(INSTALL) -m644 $(clou) $(DIR_MAN)/man$(subst .,,$(suffix $(clou)))/ && ) ) ) \
	$(call msg_always,printf "%b\n" "i: now if you wish you can suid $(DIR_BIN)/$(PRJNAME)" && ) \
	$(call msg_always,printf "%b$(NL)" "done.$(CL)" )

install: do-install install-local

install-local: | do-install

do-uninstall: | uninstall-local
	@$(call msg_always,printf "%b\n" "i: you need to become root to do this" && ) \
	$(if $(CLOU_BIN), $(call invoke,/bin/rm -f $(foreach clou,$(CLOU_BIN),$(DIR_BIN)/$(patsubst %$(EXEC_NAME),$(PRJNAME),$(clou)) && ) ) ) \
	$(if $(CLOU_LIB), $(call invoke,/bin/rm -f $(addprefix $(DIR_LIB)/,$(notdir $(CLOU_LIB))) && ) ) \
	$(if $(CLOU_MODULES), $(call invoke,/bin/rm -rf $(DIR_LIB)/$(PRJNAME) && ) ) \
	$(if $(CLOU_CONF), \
		$(call invoke,/bin/rm -f $(addprefix $(DIR_SYSCONF)/,$(foreach clou,$(CLOU_CONF),$(or $(call second,$(clou)),$(notdir $(clou))) ) ) && \
		$(if $(call second,$(firstword $(CLOU_CONF))),/bin/rm -rf $(dir $(DIR_SYSCONF)/$(call second,$(firstword $(CLOU_CONF)))) && ) ) \
	) \
	$(if $(CLOU_SHARE), $(call invoke,/bin/rm -rf $(DIR_SHARE)/$(PRJNAME) && ) ) \
	$(if $(CLOU_LOCALSTATE), $(call invoke,/bin/rm -rf $(DIR_LOCALSTATE)/$(PRJNAME) && ) ) \
	$(if $(CLOU_INCLUDE), $(call invoke,/bin/rm -fr $(addprefix $(DIR_HEADERS)/,$(notdir $(CLOU_INCLUDE))) && ) ) \
	$(if $(CLOU_DOC), $(call invoke,/bin/rm -rf $(DIR_DOC) && ) ) \
	$(if $(CLOU_BIN), $(call invoke,$(foreach clou,$(CLOU_MAN),/bin/rm -f $(DIR_MAN)/man$(subst .,,$(suffix $(clou)))/$(notdir $(clou)) && ) ) ) \
	$(call msg_always,printf "%b$(NL)" "done.$(CL)" )

uninstall: uninstall-local do-uninstall

uninstall-local:

