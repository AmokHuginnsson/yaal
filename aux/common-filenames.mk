$(foreach IT,$(TARGETS),$(eval $(call BUILD_TARGET,$(IT))))
$(foreach IT,$(DIRS),$(eval $(call MAKE_DIR,$(IT))))

