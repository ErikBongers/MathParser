SRCDIR=src
DESTDIR=www
SRCS=$(wildcard $(SRCDIR)/*.cpp)
# OBS=$(SRCS:.cpp=.o)
CC=emcc
CXX=emcc
BUILDDIR=build
OBS = $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SRCS))
DEPS = $(OBS:.o=.d)

$(info $(shell mkdir $(BUILDDIR) 2> nul))

$(DESTDIR)/hullo.js: $(OBS)
	$(CXX) -o $(DESTDIR)/hullo.js $(OBS) -O0 -s WASM=1 --shell-file html_template/shell_minimal.html -s NO_EXIT_RUNTIME=1  -s "EXPORTED_RUNTIME_METHODS=['ccall', 'cwrap']"

-include $(DEPS)

$(BUILDDIR)/%.o : $(SRCDIR)/%.cpp
# 	-@mkdir $(@D) 2> nul
	$(CXX) -c $< -o $@ -MMD -MP

.PHONY: clean

clean:
	rmdir /s /q build
	del /q "$(DESTDIR)\hullo.wasm"
	del /q "$(DESTDIR)\hullo.js"