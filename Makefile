# Set you prefererred CFLAGS/compiler compiler here.
# Our github runner provides gcc-10 by default.
CC ?= cc
CFLAGS ?= -g -Wall -O2
CXX ?= c++
CXXFLAGS ?= -g -Wall -O2
CARGO ?= cargo
RUSTFLAGS ?= -g

# this target should build all executables for all tests
all:
	@echo "Please set a concrete build command here"
	false

# C example:
#all: librw_1.so librw_2.so
#librw_1.so: task-1_1.c
#	$(CC) $(CFLAGS) -shared -fPIC -ldl -o $@ $<
#
#librw_2.so: task-1_2.c
#	$(CC) $(CFLAGS) -shared -fPIC -ldl -o $@ $<

# Rust example:
#all:
#	$(CARGO) build --release

# Usually there is no need to modify this
check: all
	$(MAKE) -C tests check
