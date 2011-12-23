// This code is licensed under the New BSD license.
// See LICENSE.txt for more details.
#include <iostream>

#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Host.h"

#include "clang/Frontend/DiagnosticOptions.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"

#include "clang/Basic/LangOptions.h"
#include "clang/Basic/FileSystemOptions.h"

#include "clang/Basic/SourceManager.h"
#include "clang/Lex/HeaderSearch.h"
#include "clang/Basic/FileManager.h"

#include "clang/Frontend/HeaderSearchOptions.h"
#include "clang/Frontend/Utils.h"

#include "clang/Basic/TargetOptions.h"
#include "clang/Basic/TargetInfo.h"

#include "clang/Lex/Preprocessor.h"
#include "clang/Frontend/PreprocessorOptions.h"
#include "clang/Frontend/FrontendOptions.h"
#include "clang/Frontend/CompilerInstance.h"


int main()
{
    clang::DiagnosticOptions diagnosticOptions;
    clang::TextDiagnosticPrinter *pTextDiagnosticPrinter =
        new clang::TextDiagnosticPrinter(
            llvm::outs(),
            diagnosticOptions);
    llvm::IntrusiveRefCntPtr<clang::DiagnosticIDs> pDiagIDs;
    clang::DiagnosticsEngine *pDiagnosticsEngine =
        new clang::DiagnosticsEngine(pDiagIDs, pTextDiagnosticPrinter);

    clang::LangOptions languageOptions;
    clang::FileSystemOptions fileSystemOptions;
    clang::FileManager fileManager(fileSystemOptions);

    clang::SourceManager sourceManager(
        *pDiagnosticsEngine,
        fileManager);

    clang::HeaderSearch headerSearch(fileManager, *pDiagnosticsEngine);
    clang::HeaderSearchOptions headerSearchOptions;

    clang::TargetOptions targetOptions;
    targetOptions.Triple = llvm::sys::getDefaultTargetTriple();
    clang::TargetInfo *pTargetInfo = 
        clang::TargetInfo::CreateTargetInfo(
            *pDiagnosticsEngine,
            targetOptions);

    clang::CompilerInstance compInst;

    clang::Preprocessor preprocessor(
        *pDiagnosticsEngine,
        languageOptions,
        pTargetInfo,
        sourceManager,
        headerSearch,
        compInst);


    clang::PreprocessorOptions preprocessorOptions;
    // disable predefined Macros so that you only see the tokens from your 
    // source file. Note, this has some nasty side-effects like also undefning
    // your archictecture and things like that.
    //preprocessorOptions.UsePredefines = false;

    clang::FrontendOptions frontendOptions;
    clang::InitializePreprocessor(
        preprocessor,
        preprocessorOptions,
        headerSearchOptions,
        frontendOptions);

    // Note: Changed the file from tutorial2.
    const clang::FileEntry *pFile = fileManager.getFile("testInclude.c");
    sourceManager.createMainFileID(pFile);
    preprocessor.EnterMainSourceFile();
    pTextDiagnosticPrinter->BeginSourceFile(languageOptions, &preprocessor);

    clang::Token token;
    do {
        preprocessor.Lex(token);
        if( pDiagnosticsEngine->hasErrorOccurred())
        {
            break;
        }
        preprocessor.DumpToken(token);
        std::cerr << std::endl;
    } while( token.isNot(clang::tok::eof));
    pTextDiagnosticPrinter->EndSourceFile();

    return 0;
}
