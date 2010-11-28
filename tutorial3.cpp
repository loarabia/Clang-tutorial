// This code is licensed under the New BSD license.
// See LICENSE.txt for more details.
#include <iostream>

#include "llvm/Support/raw_ostream.h"
#include "llvm/System/Host.h"

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


int main()
{
	clang::DiagnosticOptions diagnosticOptions;
	clang::TextDiagnosticPrinter *pTextDiagnosticPrinter =
		new clang::TextDiagnosticPrinter(
			llvm::outs(),
			diagnosticOptions);
	llvm::IntrusiveRefCntPtr<clang::DiagnosticIDs> pDiagIDs;
	clang::Diagnostic diagnostic(pDiagIDs, pTextDiagnosticPrinter);

	clang::LangOptions languageOptions;
	clang::FileSystemOptions fileSystemOptions;
	clang::FileManager fileManager(fileSystemOptions);

	clang::SourceManager sourceManager(
        diagnostic,
        fileManager);
	clang::HeaderSearch headerSearch(fileManager);

	clang::HeaderSearchOptions headerSearchOptions;

	clang::TargetOptions targetOptions;
	targetOptions.Triple = llvm::sys::getHostTriple();

	clang::TargetInfo *pTargetInfo = 
		clang::TargetInfo::CreateTargetInfo(
			diagnostic,
			targetOptions);

	clang::ApplyHeaderSearchOptions(
		headerSearch,
		headerSearchOptions,
		languageOptions,
		pTargetInfo->getTriple());

	clang::Preprocessor preprocessor(
		diagnostic,
		languageOptions,
		*pTargetInfo,
		sourceManager,
		headerSearch);

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
	preprocessor.EnterMainSourceFile();
    pTextDiagnosticPrinter->BeginSourceFile(languageOptions, &preprocessor);

	clang::Token token;
	do {
		preprocessor.Lex(token);
		if( diagnostic.hasErrorOccurred())
		{
			break;
		}
		preprocessor.DumpToken(token);
		std::cerr << std::endl;
	} while( token.isNot(clang::tok::eof));
    pTextDiagnosticPrinter->EndSourceFile();

	return 0;
}
