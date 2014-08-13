
#include <iostream>
#include <vector>

#include "llvm/Support/CommandLine.h"

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/Parse/Parser.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

using namespace clang::driver;
using namespace clang::tooling;

/******************************************************************************
 *
 *****************************************************************************/
namespace tooling {

/******************************************************************************
 *
 *****************************************************************************/
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
            if( vd->isFileVarDecl() && !vd->hasExternalStorage() )
            {
                std::cerr << "Read top-level variable decl: '";
                std::cerr << vd->getDeclName().getAsString() ;
                std::cerr << std::endl;
            }
        }
        return true;
    }
};

/******************************************************************************
 *
 *****************************************************************************/
class MyFactory
{
  public:
    clang::ASTConsumer *newASTConsumer() {
      return new MyASTConsumer();
      }

};

} // Namespace tooling

/******************************************************************************
 *
 *****************************************************************************/
int main(int argc, const char **argv)
{
  CommonOptionsParser OptionsParser(argc, argv);
  ClangTool Tool(OptionsParser.GetCompilations(),
                 OptionsParser.GetSourcePathList());
  tooling::MyFactory Factory;
  Tool.run(newFrontendActionFactory(&Factory));
  return 0;
}
