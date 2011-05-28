include ../Makefile.global

DIRS = tsdumper

all: compile

compile:
	for i in $(DIRS); do make -C $$i; done

clean: clean-all
	
clean-all:
	for i in $(DIRS); do make -C $$i clean; done
