# $CVSHeader$
# This is wrapper around GNU make.

all: Makefile
	@make banner MSG="Invoking GNU make (gmake) ..."
	@gmake all

Makefile: Makefile.in stdhapirc.in config.h.in configure
	@make banner MSG='Generating \`Makefile'"' and rest of necessary files ..."
	@./configure
	@make banner MSG='Ready for \`make'"'."

configure: configure.ac
	@make banner MSG='Generating \`configure'"' script ... "
	@autoconf 2> /dev/null
	@echo "Done."

nodebug: banner 
	@gmake nodebug

dep:
	@make banner MSG='Generating dependicies ...'
	@gmake dep

tags:
	@make banner MSG='Generating tags ...'
	@gmake tags

install:
	@make banner MSG='Installing ...'
	@gmake install

clean:
	@make banner MSG='Cleaning ...'
	@gmake clean

mrproper:
	@make banner MSG='Extra cleaning ...'
	@gmake mrproper

purge:
	@make banner MSG='Purging ...'
	@gmake purge

banner:
	@echo "[1m<<<[m make: ${MSG} [1m>>>[m"

# vim: ft=make
