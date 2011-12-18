/******************************************************************************
 * The Indenter is responsible for taking the optons and arguments, creating
 * the required LLVM and Clang types and performing the task at hand.
 *****************************************************************************/
#ifndef INDENTER_H 
#define INDENTER_H 

#include <string>

#include "llvm/Support/Path.h"
#include "llvm/Support/Host.h"

#include "clang/Frontend/CompilerInstance.h"

using std::string;

using llvm::sys::Path;

using clang::CompilerInstance;

/******************************************************************************
 *
 *****************************************************************************/
class Indenter {
    private:
        Path InputFile;
        CompilerInstance compilerInstance;
    public:
        Indenter(string);
        void initializePreprocessor();
};

#endif
