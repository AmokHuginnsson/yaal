tags: $(SRCS) $(HDRS)
	@printf "%b" "Rehashing tags ... "; \
	cd $(DIR_ROOT) && $(CTAGS) && \
	printf "%b\n" "done."

