CXX := clang++
LLVMCOMPONENTS := cppbackend
RTTIFLAG := -fno-rtti
LLVMCONFIG := ~/Documents/src/llvm/build/Release/bin/llvm-config

CXXFLAGS := -I$(shell $(LLVMCONFIG) --src-root)/tools/clang/include -I$(shell $(LLVMCONFIG) --obj-root)/tools/clang/include $(shell $(LLVMCONFIG) --cxxflags) $(RTTIFLAG)
LLVMLDFLAGS := $(shell $(LLVMCONFIG) --ldflags --libs $(LLVMCOMPONENTS))

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
    CIBasicRecursiveASTVisitor.cpp \
    CIrewriter.cpp \
		ToolingTutorial.cpp \
		CommentHandling.cpp

OBJECTS = $(SOURCES:.cpp=.o)
EXES = $(OBJECTS:.o=)
CLANGLIBS = \
				-lclangTooling\
				-lclangFrontendTool\
				-lclangFrontend\
				-lclangDriver\
				-lclangSerialization\
				-lclangCodeGen\
				-lclangParse\
				-lclangSema\
				-lclangStaticAnalyzerFrontend\
				-lclangStaticAnalyzerCheckers\
				-lclangStaticAnalyzerCore\
				-lclangAnalysis\
				-lclangARCMigrate\
				-lclangRewriteFrontend\
				-lclangRewriteCore\
				-lclangEdit\
				-lclangAST\
				-lclangLex\
				-lclangBasic\
				$(shell $(LLVMCONFIG) --libs)

all: $(OBJECTS) $(EXES)

%: %.o
	$(CXX) -o $@ $< $(CLANGLIBS) $(LLVMLDFLAGS)

clean:
	-rm -f $(EXES) $(OBJECTS) *~
