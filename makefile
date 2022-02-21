# normal compile flags
CXX = g++
CXX_FLAGS = -std=c++17 -lpthread -lm -O3 -fPIC
CXX_INCLUDE = -I./include
CXX_LIB = -L./lib -Wl,-rpath=$(shell pwd)/lib $(patsubst lib%.so,-l%,$(notdir $(wildcard ./lib/*.so)))

# for auto generation of dependency file 
OBJDIR := obj
DEPDIR := $(OBJDIR)/.deps
DEPFLAGS = -MT $@ -MD -MP -MF $(DEPDIR)/$*.d
COMPILE.o = $(CXX) $(DEPFLAGS) $(CXX_FLAGS) $(CXX_INCLUDE) -c $< -o ./$(OBJDIR)/$@
COMPILE.so = $(CXX) $(CXX_FLAGS) $(CXX_INCLUDE) -shared -fPIC -o $@ $^
COMPILE.exe = $(CXX) $(CXX_FLAGS) $(CXX_LIB) $(CXX_INCLUDE) -o $@ $^

# list of objects needed to be built
SRCS = $(notdir $(wildcard ./src/*.cxx ./control/*.cxx))
OBJECTS = $(patsubst %.cxx,%.o,$(notdir $(wildcard ./src/*.cxx ./control/*.cxx)))
LIBS = ./lib/libAntColonySim.so
EXECUTABLES = $(patsubst %.cxx,./bin/%,$(notdir $(wildcard ./control/*.cxx)))

VPATH = ./control:./src:./include:./obj

# build all objects
release : 
	@mkdir -p $(OBJDIR) bin lib
	+make $(OBJECTS)
	+make $(LIBS)
	+make $(EXECUTABLES)

# build .o file
%.o : %.cxx $(DEPDIR)/%.d | $(DEPDIR)
	@$(COMPILE.o)

# for dependency files
$(DEPDIR): ; @mkdir -p $@

DEPFILES := $(SRCS:%.cxx=$(DEPDIR)/%.d)
$(DEPFILES):
include $(wildcard $(DEPFILES))

# build LIBS
./lib/libAntColonySim.so : $(patsubst %.cxx,./obj/%.o,$(notdir $(wildcard ./src/*.cxx)))
	@$(COMPILE.so)

# build executable file 
./bin/% : ./$(OBJDIR)/%.o
	@$(COMPILE.exe)

# phony objects
.PHONY : clean
clean :
	-rm ./bin/* ./obj/* ./lib/* ./obj/.deps/*
