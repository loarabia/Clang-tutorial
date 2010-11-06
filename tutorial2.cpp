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

#include "clang/Basic/TargetOptions.h"
#include "clang/Basic/TargetInfo.h"

#include "clang/Lex/Preprocessor.h"


int main()
{
	clang::DiagnosticOptions diagnosticOptions;
	clang::TextDiagnosticPrinter *pTextDiagnosticPrinter =
		new clang::TextDiagnosticPrinter(
			llvm::outs(),
			diagnosticOptions);
	clang::Diagnostic diagnostic(pTextDiagnosticPrinter);

	clang::LangOptions languageOptions;
	clang::FileManager fileManager;
    clang::FileSystemOptions fileSystemOptions;


	clang::SourceManager sourceManager(
        diagnostic,
        fileManager,
        fileSystemOptions);
	clang::HeaderSearch headerSearch(fileManager, fileSystemOptions);

	clang::TargetOptions targetOptions;
	targetOptions.Triple = llvm::sys::getHostTriple();

	clang::TargetInfo *pTargetInfo = 
		clang::TargetInfo::CreateTargetInfo(
			diagnostic,
			targetOptions);

	clang::Preprocessor preprocessor(
		diagnostic,
		languageOptions,
		*pTargetInfo,
		sourceManager,
		headerSearch);


	const clang::FileEntry *pFile = fileManager.getFile(
        "test.c",
        fileSystemOptions);
	sourceManager.createMainFileID(pFile);
	preprocessor.EnterMainSourceFile();

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

	return 0;
}
