CC=emcc
CXX=emcc

BUILDDIR=build
SRC_DIR=src
SRC_LIB_DIR=D:\Documents\Programming\WPFandXNA\MathParser\MathParserDll
DESTDIR=www

SRCS=$(wildcard $(SRC_DIR)/*.cpp)
SRCS_LIB= $(wildcard $(SRC_LIB_DIR)/*.cpp)

DEPFLAGS=-MMD -MP
CFLAGS=-I "$(SRC_LIB_DIR)" -D__WASM__ $(DEPFLAGS) -std=c++20

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
	$(CXX) -o $(DESTDIR)/mathparser.js $(OBS) $(OBS_LIB) -O0 -s WASM=1 --shell-file html_template/shell_minimal.html -s NO_EXIT_RUNTIME=1  -s "EXPORTED_RUNTIME_METHODS=['ccall', 'cwrap']"
	rename "$(DESTDIR)\mathparser.js" mathparser.wasm.js

-include $(DEPS)

$(BUILDDIR)/%.o : $(SRC_DIR)/%.cpp
	$(CXX) -c $< -o $@ $(CFLAGS) 

$(BUILDDIR)/lib/%.o : $(SRC_LIB_DIR)/%.cpp
	$(CXX) -c $< -o $@ $(CFLAGS)

.PHONY: clean

clean:
	rmdir /s /q build
	del /q "$(DESTDIR)\mathparser.wasm"
	del /q "$(DESTDIR)\mathparser.wasm.js"