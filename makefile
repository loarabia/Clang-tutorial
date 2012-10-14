CXX := clang++
LLVMCOMPONENTS := cppbackend
RTTIFLAG := -fno-rtti
LLVMCONFIG := ../cl31/bin/llvm-config

CXXFLAGS := $(shell $(LLVMCONFIG) --cxxflags) $(RTTIFLAG)
LLVMLDFLAGS := $(shell $(LLVMCONFIG) --ldflags --libs $(LLVMCOMPONENTS))
DDD := $(shell echo $(LLVMLDFLAGS))
SOURCES = tutorial1.cpp \
    tutorial2.cpp \
    tutorial3.cpp \
    tutorial4.cpp \
    tutorial6.cpp \
    CItutorial1.cpp \
    CItutorial2.cpp \
    CItutorial3.cpp \
    CItutorial4.cpp \
    CItutorial6.cpp \
    CIBasicRecursiveASTVisitor.cpp

OBJECTS = $(SOURCES:.cpp=.o)
EXES = $(OBJECTS:.o=)
CLANGLIBS = \
    -lclangFrontend \
    -lclangEdit \
    -lclangParse \
    -lclangSema \
    -lclangAnalysis \
    -lclangAST \
    -lclangLex \
    -lclangBasic \
    -lclangDriver \
    -lclangSerialization \
    -lLLVMMC \
    -lLLVMSupport \

all: $(OBJECTS) $(EXES)

%: %.o
	$(CXX) -o $@ $< $(CLANGLIBS) $(LLVMLDFLAGS)

clean:
	-rm -f $(EXES) $(OBJECTS) *~
