tags: $(SRCS) $(HDRS)
	@echo -n "Rehashing tags ... "; \
	cd $(DIR_ROOT) && $(CTAGS) && \
	echo "done."

