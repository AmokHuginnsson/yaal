# This is wrapper around GNU make.

all: Makefile
	@make banner
	@echo "Invoking GNU make (gmake)."
	@gmake all

Makefile: configure
	@make banner
	@echo "Generating \`Makefile' and rest of necessary files."
	@./configure
	@echo "Ready for \`make'."

configure:
	@make banner
	@echo -n "Generating \`configure' script ... "
	-@autoconf 2> /dev/null
	@echo "done."

nodebug: banner
	@gmake nodebug

dep: banner
	@gmake dep

tags: banner
	@gmake tags

install: banner
	@gmake install

clean: banner
	@gmake clean

mrproper: banner
	@gmake mrproper

purge: banner
	@gmake purge

banner:
	@echo "[1m!!![m It is only a wrapper, use GNU make (gmake). [1m!!![m"

# vim: ft=make
