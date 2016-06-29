COLOR_ON = color
COLOR_OFF = 
CC = $(COLOR_ON)gcc
#CC = $(COLOR_OFF)gcc
LD = ld

all:
	@$(MAKE) -C discovery -f Makefile
	@$(MAKE) -C device_management -f makefile
.PHONY: clean
clean:
	@$(MAKE) -C discovery -f Makefile clean
	@$(MAKE) -C device_management -f Makefile clean