.DEFAULT:
	@if [ "$(suffix $(<))" != ".$(DS)" ] ; then \
	echo -n "$(BOLD)$(RED)"; \
	echo "Lack of \`$(<)'." | tee -a make.log; \
	echo -n "$(RS)"; fi && false

