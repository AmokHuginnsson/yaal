ifndef TARGET_DEFAULT_INCLUDED
TARGET_DEFAULT_INCLUDED=1

.DEFAULT:
	@if [ "$(suffix $(<))" != ".$(DS)" -a "$(suffix $(<))" != ".mk" ] ; then \
	printf "%b" "$(BOLD)$(RED)"; \
	printf "%b\n" "Lack of \`$(<)'." | tee -a make.log; \
	printf "%b" "$(RS)"; fi && false

endif

