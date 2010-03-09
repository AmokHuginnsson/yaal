define PREPARE_VARIABLES
	DIR_$(1) := $(1)
	DIRS += $$(DIR_BUILD)/$$(DIR_$(1))
ifeq ($(CURDIR),$(DIR_BUILD))
	SONAME_$(1) := lib$(1).so.$$(VERSION).$$(SUBVERSION)
	TARGET_$(1) := lib$(1).so.$$(RELEASE)
	TARGETS += $$(TARGET_$(1))
	HDRS += $$(strip $$(sort $$(shell cd $$(DIR_ROOT) && $$(FIND) ./$$(DIR_$(1)) -name "*.$(HS)")))
	SRC_$(1) = $$(strip $$(sort $$(shell cd $$(DIR_ROOT) && $$(FIND) ./$$(DIR_$(1)) -name "*.$(SS)")))
	SRCS += $$(SRC_$(1))
	OBJS_$(1) := $$(patsubst %.$(SS),%.$(OS),$$(SRC_$(1)))
	OBJS += $$(OBJS_$(1))
endif
endef

define CONFIGURE_DRIVER
	SONAME_$(1) := lib$(1)_driver.so.$$(VERSION).$$(SUBVERSION)
	TARGET_$(1)_driver := lib$(1)_driver.so.$$(RELEASE)
	TARGETS += $$(TARGET_$(1)_driver)
	SRCS += $$(patsubst %,$$(DIR_ROOT)/$$(DIR_dbwrapper)/%.$(SS),$(1)_driver)
	OBJS_$(1)_driver := ./$$(patsubst %,$$(DIR_dbwrapper)/%.$(OS), $(1)_driver)
	OBJS += $$(OBJS_$(1)_driver)
endef

define MAKE_DIR
$(1):
	@umask 022;/bin/mkdir -p $(1)
endef

