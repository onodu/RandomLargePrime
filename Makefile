MAKEFILE = Makefile

.PHONY: all release debug test


all : release debug

INCPATH := -I. -isystemK:\1\0\source\boost_1_67_0
LIBS := 
PROJECT_NAME := random-large-prime

release :
	$(MAKE) -f $(MAKEFILE)2\
  "TARGET_DIR = release  "\
  "CXXFLAGS = -O2 -std=c++14 -Werror=return-type"\
  "LFLAGS = -s"\
  "INCPATH = $(INCPATH)"\
  "LIBS = $(LIBS)"\
  "PROJECT_NAME = $(PROJECT_NAME)"


debug :
	$(MAKE) -f $(MAKEFILE)2\
  "TARGET_DIR = debug "\
  "CXXFLAGS = -Wall -Wextra -Wpedantic -g -O0 -std=c++14 -Werror=return-type"\
  "LFLAGS = "\
  "INCPATH = $(INCPATH)"\
  "LIBS = $(LIBS)"\
  "PROJECT_NAME = $(PROJECT_NAME)"

test : 
	$(MAKE) -f $(MAKEFILE)2 test