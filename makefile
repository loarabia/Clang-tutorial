CXX = g++
LLVMFLAGS = $(shell llvm-config --cxxflags --ldflags --libs)
SOURCES = tutorial1.cpp \
    tutorial2.cpp \
    tutorial3.cpp \
    tutorial4.cpp \
    tutorial6.cpp
EXES = $(SOURCES:.cpp=)
CLANGLIBS = -lclangParse \
    -lclangSema \
    -lclangAnalysis \
    -lclangAST \
	-lclangFrontend \
	-lclangLex \
	-lclangBasic \
	-lLLVMSupport \
	-lLLVMSystem \

all: $(SOURCES) $(EXES)

$(EXES): $(SOURCES)
	$(CXX) -o $@ $@.cpp $(CLANGLIBS) $(LLVMFLAGS)

clean:
	-rm -f $(EXES) *~
