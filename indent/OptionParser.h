/******************************************************************************
 * The OptionParser recieves the options and arguments from the llvm 
 * commandLine library. It is responsible also for providing any helpers or
 * additional processing on the options and arguments before they are
 * consumed by the Indenter.
 *****************************************************************************/
#ifndef OPTIONPARSER_H
#define OPTIONPARSER_H

#include "llvm/Support/CommandLine.h"
#include <string>

using namespace std;
using namespace llvm;

/******************************************************************************
 *
 *****************************************************************************/
class OptionParser {

    public:
        static string InputFilename;
};


#endif
