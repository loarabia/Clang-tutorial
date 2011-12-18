#include <string>

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Basic/TargetOptions.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/AST/ASTConsumer.h"

#include "Indenter.h"

using std::string;


/******************************************************************************
 *
 *****************************************************************************/
Indenter::Indenter(string inputFilename) : 
    InputFile(inputFilename),
    compilerInstance()
{
    InputFile.createTemporaryFileOnDisk();
}

/******************************************************************************
 *
 *****************************************************************************/
void Indenter::initializePreprocessor()
{
    using clang::CompilerInstance;
    using clang::TargetOptions;
    using clang::TargetInfo;
    using clang::ASTConsumer;
    
    compilerInstance.createDiagnostics(0,NULL);

    TargetOptions to;
    to.Triple = llvm::sys::getDefaultTargetTriple();
    TargetInfo *pti = 
        TargetInfo::CreateTargetInfo(compilerInstance.getDiagnostics(), to);
    compilerInstance.setTarget(pti);

    compilerInstance.createFileManager();
    compilerInstance.createSourceManager(compilerInstance.getFileManager());
    compilerInstance.createPreprocessor();

    ASTConsumer *astConsumer = new ASTConsumer();
    compilerInstance.setASTConsumer(astConsumer);
    compilerInstance.createASTContext();
}
