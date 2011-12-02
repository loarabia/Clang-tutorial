// This code is licensed under the New BSD license.
// See LICENSE.txt for more details.
#include <iostream>

#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/Casting.h"

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

#include "clang/Basic/IdentifierTable.h"
#include "clang/Basic/Builtins.h"

#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/Sema/Sema.h"
#include "clang/AST/DeclBase.h"
#include "clang/AST/Type.h"
#include "clang/AST/Decl.h"
#include "clang/Sema/Lookup.h"
#include "clang/Sema/Ownership.h"
#include "clang/AST/DeclGroup.h"

#include "clang/Parse/Parser.h"

#include "clang/Parse/ParseAST.h"
#include "clang/Frontend/CompilerInstance.h"

class MyASTConsumer : public clang::ASTConsumer
{
public:
    MyASTConsumer() : clang::ASTConsumer() { }
    virtual ~MyASTConsumer() { }

    virtual bool HandleTopLevelDecl( clang::DeclGroupRef d)
    {
        static int count = 0;
        clang::DeclGroupRef::iterator it;
        for( it = d.begin(); it != d.end(); it++)
        {
            count++;
            clang::VarDecl *vd = llvm::dyn_cast<clang::VarDecl>(*it);
            if(!vd)
            {
                continue;
            }
            std::cout << vd << std::endl;
            if( vd->isFileVarDecl() && vd->hasExternalStorage() )
            {
                std::cerr << "Read top-level variable decl: '";
                std::cerr << vd->getDeclName().getAsString() ;
                std::cerr << std::endl;
            }
        }
        return true;
    }
};

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
	clang::Diagnostic diagnostic(pDiagnosticsEngine);

	clang::LangOptions languageOptions;
	clang::FileSystemOptions fileSystemOptions;
	clang::FileManager fileManager(fileSystemOptions);

	clang::SourceManager sourceManager(
        *pDiagnosticsEngine,
        fileManager);
	clang::HeaderSearch headerSearch(fileManager, *pDiagnosticsEngine);

	clang::HeaderSearchOptions headerSearchOptions;
	// <Warning!!> -- Platform Specific Code lives here
	// This depends on A) that you're running linux and
	// B) that you have the same GCC LIBs installed that
	// I do. 
	// Search through Clang itself for something like this,
	// go on, you won't find it. The reason why is Clang
	// has its own versions of std* which are installed under 
	// /usr/local/lib/clang/<version>/include/
	// See somewhere around Driver.cpp:77 to see Clang adding
	// its version of the headers to its include path.
	headerSearchOptions.AddPath("/usr/include/linux",
			clang::frontend::Angled,
			false,
			false,
			false);
	headerSearchOptions.AddPath("/usr/include/c++/4.4/tr1",
			clang::frontend::Angled,
			false,
			false,
			false);
	headerSearchOptions.AddPath("/usr/include/c++/4.4",
			clang::frontend::Angled,
			false,
			false,
			false);
	// </Warning!!> -- End of Platform Specific Code

	clang::TargetOptions targetOptions;
	targetOptions.Triple = llvm::sys::getDefaultTargetTriple();

	clang::TargetInfo *pTargetInfo = 
		clang::TargetInfo::CreateTargetInfo(
            *pDiagnosticsEngine,
			targetOptions);

	clang::ApplyHeaderSearchOptions(
		headerSearch,
		headerSearchOptions,
		languageOptions,
		pTargetInfo->getTriple());

    clang::CompilerInstance compInst;

	clang::Preprocessor preprocessor(
        *pDiagnosticsEngine,
		languageOptions,
		pTargetInfo,
		sourceManager,
		headerSearch,
        compInst);

	clang::PreprocessorOptions preprocessorOptions;
	clang::FrontendOptions frontendOptions;
	clang::InitializePreprocessor(
		preprocessor,
		preprocessorOptions,
		headerSearchOptions,
		frontendOptions);
		
	const clang::FileEntry *pFile = fileManager.getFile(
        "test.c");
	sourceManager.createMainFileID(pFile);
	//preprocessor.EnterMainSourceFile();

    const clang::TargetInfo &targetInfo = *pTargetInfo;

    clang::IdentifierTable identifierTable(languageOptions);
    clang::SelectorTable selectorTable;

    clang::Builtin::Context builtinContext;
    builtinContext.InitializeTarget(targetInfo);
    clang::ASTContext astContext(
        languageOptions,
        sourceManager,
        pTargetInfo,
        identifierTable,
        selectorTable,
        builtinContext,
        0 /* size_reserve*/);
   // clang::ASTConsumer astConsumer;
   MyASTConsumer astConsumer;

    clang::Sema sema(
        preprocessor,
        astContext,
        astConsumer);
    //sema.Initialize();

   //MySemanticAnalisys mySema( preprocessor, astContext, astConsumer);

    //clang::Parser parser( preprocessor, sema);
    //parser.ParseTranslationUnit();
    pTextDiagnosticPrinter->BeginSourceFile(languageOptions, &preprocessor);
    clang::ParseAST(preprocessor, &astConsumer, astContext); 
    pTextDiagnosticPrinter->EndSourceFile();
	return 0;
}
