tags: $(SRCS) $(HDRS)
	@echo -n "Rehashing tags ... "; \
	$(CTAGS)	-R --langmap="c++:.$(HS).$(SS)" -h ".$(HS)" --languages="c++" \
						--c++-kinds="+p+x" --format=2 --fields="afikKlmnsSz" \
						--extra="+f+q"; \
	echo "done."

