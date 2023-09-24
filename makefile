NAME = CONVERT
DESCRIPTION = "A program for converting units"
COMPRESSED = YES
COMPRESSED_MODE = zx0
ARCHIVED = YES
CFLAGS = "-O3"
CXXFLAGS = -O3 "-I./"
include $(shell cedev-config --makefile)