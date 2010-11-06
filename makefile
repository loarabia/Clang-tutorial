CXX = g++
CXXFLAGS = -g -c -Wall -ansi -pedantic -fno-rtti
DEFS =  -D__STDC_LIMIT_MACROS=0 \
	-D__STDC_CONSTANT_MACROS=0
INCLUDES = -I/usr/local/include
SOURCES = tutorial1.cpp \
    tutorial2.cpp \
    tutorial3.cpp \
    tutorial4.cpp \
    tutorial6.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXES = $(OBJECTS:.o=)
LIBS =  -ldl \
	-lpthread \
    -lclangParse \
    -lclangSema \
    -lclangAnalysis \
    -lclangAST \
	-lclangFrontend \
	-lclangLex \
	-lclangBasic \
	-lLLVMSupport \
	-lLLVMSystem \
    -lLLVMCore \
	-lLLVMMC 

all: $(SOURCES) $(EXES)

$(EXES): $(OBJECTS)
	$(CXX) -o $@ $(LIBS) $@.o 

.cpp.o:
	$(CXX) $(CXXFLAGS) $(DEFS) $(LOCAL_INCLUDES) $< -o $@

clean:
	-rm $(EXES)
	-rm $(OBJECTS)
