/***   CItutorial6.cpp   *****************************************************
 * This code is licensed under the New BSD license.
 * See LICENSE.txt for details.
 * 
 * The CI tutorials remake the original tutorials but using the
 * CompilerInstance object which has as one of its purpose to create commonly
 * used Clang types.
 *****************************************************************************/
#include <iostream>

#include "llvm/Support/Host.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/OwningPtr.h"
#include "llvm/Support/raw_ostream.h"

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Basic/TargetOptions.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/Parse/ParseAST.h"


// RecursiveASTVisitor is is the big-kahuna visitor that traverses
// everything in the AST.
class MyRecursiveASTVisitor
    : public clang::RecursiveASTVisitor<MyRecursiveASTVisitor> {

 public:
  bool VisitTypedefDecl(clang::TypedefDecl *d);

};


bool MyRecursiveASTVisitor::VisitTypedefDecl(clang::TypedefDecl *d) {
  llvm::errs() << "Visiting " << d->getDeclKindName() << " "
               << d->getName() << "\n";
  return true; // returning false aborts the traversal
}


class MyASTConsumer : public clang::ASTConsumer {
 public:
    virtual bool HandleTopLevelDecl(clang::DeclGroupRef d);
};


bool MyASTConsumer::HandleTopLevelDecl(clang::DeclGroupRef d) {
  MyRecursiveASTVisitor rv;
  typedef clang::DeclGroupRef::iterator iter;
  for (iter b = d.begin(), e = d.end(); b != e; ++b) {
    rv.TraverseDecl(*b);
  }
  return true; // keep going
}


int main()
{
  using clang::CompilerInstance;
  using clang::TargetOptions;
  using clang::TargetInfo;
  using clang::FileEntry;

  CompilerInstance ci;
  ci.createDiagnostics(0,NULL);

  llvm::IntrusiveRefCntPtr<TargetOptions> pto( new TargetOptions());
  pto->Triple = llvm::sys::getDefaultTargetTriple();
  llvm::IntrusiveRefCntPtr<TargetInfo> pti(TargetInfo::CreateTargetInfo(ci.getDiagnostics(), pto.getPtr()));
  ci.setTarget(pti.getPtr());

  ci.createFileManager();
  ci.createSourceManager(ci.getFileManager());
  ci.createPreprocessor();
  ci.getPreprocessorOpts().UsePredefines = false;
  MyASTConsumer *astConsumer = new MyASTConsumer();
  ci.setASTConsumer(astConsumer);

  ci.createASTContext();

  const FileEntry *pFile = ci.getFileManager().getFile("test.c");
  ci.getSourceManager().createMainFileID(pFile);
  ci.getDiagnosticClient().BeginSourceFile(ci.getLangOpts(),
                                           &ci.getPreprocessor());
  clang::ParseAST(ci.getPreprocessor(), astConsumer, ci.getASTContext());
  ci.getDiagnosticClient().EndSourceFile();

  return 0;
}
