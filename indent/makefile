CXX := clang++

LLVMCOMPONENTS := backend
RTTIFLAG := -fno-rtti

CXXFLAGS := $(shell llvm-config --cxxflags) $(RTTIFLAG)
LLVMLDFLAGS := $(shell llvm-config --ldflags --libs $(LLVMCOMPONENTS))

SOURCES = main.cpp \
    OptionParser.cpp \
    Indenter.cpp

OBJECTS = $(SOURCES:.cpp=.o)
CLANGLIBS = -lclangParse \
    -lclangSerialization \
    -lclangDriver \
    -lclangSema \
    -lclangAnalysis \
    -lclangAST \
	-lclangFrontend \
	-lclangLex \
    -lLLVMMC \
	-lclangBasic \
	-lLLVMSupport \

indent: $(OBJECTS)
	$(CXX) -o indent $(OBJECTS) $(CLANGLIBS) $(LLVMLDFLAGS)

%: %.o
	$(CXX) -o $@ $< $(CLANGLIBS) $(LLVMLDFLAGS)

clean:
	-rm -f $(EXES) $(OBJECTS) *~
