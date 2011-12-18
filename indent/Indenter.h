/******************************************************************************
 * The Indenter is responsible for taking the optons and arguments, creating
 * the required LLVM and Clang types and performing the task at hand.
 *****************************************************************************/
#ifndef INDENTER_H 
#define INDENTER_H 

#include <string>

#include "llvm/Support/Path.h"

using std::string;
using llvm::sys::Path;

/******************************************************************************
 *
 *****************************************************************************/
class Indenter {
    private:
        Path InputFile;
    public:
        Indenter(string);
};

#endif
