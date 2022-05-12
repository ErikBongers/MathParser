CC=emcc
CXX=emcc

BUILDDIR=build
SRC_DIR=src
SRC_LIB_DIR=D:\Documents\Programming\WPFandXNA\MathParser\MathParserDll
DESTDIR=www

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
SRCS_LIB = $(wildcard $(SRC_LIB_DIR)/*.cpp)
SRCS_LIB := $(filter-out $(SRC_LIB_DIR)/pch.cpp, $(SRCS_LIB))

DEPFLAGS=-MMD -MP
CFLAGS=-I "$(SRC_LIB_DIR)" -D__WASM__ -std=c++2b
INCLUDE_PCH = $(CFLAGS) -Winvalid-pch -include-pch "D:\Documents\Programming\WPFandXNA\MathParser\MathParserDll\pch.hpp.pch"

# local source compiled to build/
OBS = $(notdir $(SRCS))
OBS := $(OBS:.cpp=.o)
OBS := $(patsubst %.o, $(BUILDDIR)/%.o, $(OBS))

# MathParser source, compiled to build/lib/ -> see separate compile target for lib below.
OBS_LIB = $(notdir $(SRCS_LIB))
OBS_LIB := $(OBS_LIB:.cpp=.o)
OBS_LIB := $(patsubst %.o, $(BUILDDIR)/lib/%.o, $(OBS_LIB))
DEPS = $(OBS:.o=.d)

$(info $(shell mkdir $(BUILDDIR)\lib 2> nul))

$(DESTDIR)/hullo.js: $(OBS) $(OBS_LIB)
	del /q "$(DESTDIR)\mathparser.wasm.js"
	$(CXX) -o $(DESTDIR)/mathparser.js $(OBS) $(OBS_LIB) -O0 -s WASM=1 -s NO_EXIT_RUNTIME=1  -s "EXPORTED_RUNTIME_METHODS=['ccall', 'cwrap']" -s LLD_REPORT_UNDEFINED
	rename "$(DESTDIR)\mathparser.js" mathparser.wasm.js

-include $(DEPS)

$(BUILDDIR)/%.o : $(SRC_DIR)/%.cpp
	$(CXX) -c $< -o $@ $(CFLAGS) $(DEPFLAGS) $(INCLUDE_PCH)

$(BUILDDIR)/lib/%.o : $(SRC_LIB_DIR)/%.cpp
	$(CXX) -c $< -o $@ $(CFLAGS) $(DEPFLAGS) $(INCLUDE_PCH)

.PHONY: clean

clean:
	rmdir /s /q build
	del /q "$(DESTDIR)\mathparser.wasm"
	del /q "$(DESTDIR)\mathparser.wasm.js"

.PHONY: pch

pch:
	$(CXX) -c $(SRC_LIB_DIR)\pch.hpp -o $(SRC_LIB_DIR)\pch.hpp.pch $(CFLAGS)