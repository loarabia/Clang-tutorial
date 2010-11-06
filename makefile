CXX = g++
CXXFLAGS = -g -c -Wall -ansi -pedantic
DEFS =  -D__STDC_LIMIT_MACROS=0 \
	-D__STDC_CONSTANT_MACROS=0
INCLUDES = -I/usr/local/include
SOURCES = tutorial.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = tutorial
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
	-lLLVMMC 

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LIBS) -o $@ 

.cpp.o:
	$(CXX) $(CXXFLAGS) $(DEFS) $(LOCAL_INCLUDES) $< -o $@

clean:
	-rm $(EXECUTABLE)
	-rm $(OBJECTS)
