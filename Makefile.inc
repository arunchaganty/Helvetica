# vim: ft=make

# put common definitions in here
CC		 = g++
DEBUG    = -O0 -ggdb # -pg
CFLAGS	 = -Wall -I$(ROOTDIR)include/ -I$(ROOTDIR)/parser/ $(DEBUG) 
CXXFLAGS = $(CFLAGS)
LDFLAGS	 = #-g 

