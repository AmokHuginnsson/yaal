VPATH=$(shell pwd)
ARCH=$(shell isalist | awk '{print $$1}')
OS=$(shell uname -r)
VERSION=$(shell awk '/^VERSION *=|SUBVERSION *=|EXTRAVERSION *=/{VERSION=VERSION DOT $$3;DOT="."}END{print VERSION}' ../../Makefile.mk.in)

include solaris.mk
