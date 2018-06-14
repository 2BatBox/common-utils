# project name (generate executable with this name)
TARGET=common-utils

# project directories
SRCDIR=src
BINDIR=bin

CC=g++
LINKER=g++

CFLAGS=-std=c++11 -Wall -Wextra -pthread -mssse3 -g3
LDFLAGS=-lrt -pthread

SOURCES  := $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCDIR)/**/*.cpp) $(wildcard $(SRCDIR)/**/**/*.cpp)

INCLUDES := $(wildcard $(SRCDIR)/*.h) $(DPDK_INCLUDES)

OBJECTS  := $(SOURCES:%.cpp=%.o)

all: prepare $(BINDIR)/$(TARGET)

prepare:
	@mkdir -p $(BINDIR)

.cpp.o:
	@echo "Compiling "$<"..."
	$(CC) $(CFLAGS) -c $< -o $@

$(BINDIR)/$(TARGET): $(OBJECTS)
	@echo "Linking "$@"..."
	$(LINKER) -o $@ $(LDFLAGS) $(OBJECTS)

clean:
	@rm $(OBJECTS)
	@rm -rf $(BINDIR)
