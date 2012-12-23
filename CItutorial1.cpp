/***   CItutorial1.cpp   *****************************************************
 * This code is licensed under the New BSD license.
 * See LICENSE.txt for details.
 * 
 * The _CI tutorials remake the original tutorials but using the
 * CompilerInstance object which has as one of its purpose to create commonly
 * used Clang types.
 *****************************************************************************/
#include "llvm/Support/Host.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Basic/TargetOptions.h"
#include "clang/Basic/TargetInfo.h"

/******************************************************************************
 * This tutorial just shows off the steps needed to build up to a Preprocessor
 * object. Note that the order below is important.
 *****************************************************************************/
int main()
{
    using clang::CompilerInstance;
    using clang::TargetOptions;
    using clang::TargetInfo;

    CompilerInstance ci;
    ci.createDiagnostics(0,NULL);

    llvm::IntrusiveRefCntPtr<TargetOptions> pto( new TargetOptions());
    pto->Triple = llvm::sys::getDefaultTargetTriple();
    TargetInfo *pti = TargetInfo::CreateTargetInfo(ci.getDiagnostics(), *pto);
    ci.setTarget(pti);

    ci.createFileManager();
    ci.createSourceManager(ci.getFileManager());
    ci.createPreprocessor();
    return 0;
}
