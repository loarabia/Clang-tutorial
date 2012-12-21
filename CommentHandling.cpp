#include <iostream>
#include <vector>

#include "clang/Basic/SourceLocation.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/Parse/Parser.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

using namespace clang::driver;
using namespace clang::tooling;
using clang::FileID;

/******************************************************************************
 *
 *****************************************************************************/
namespace tooling {

class MyCommentHandler : public clang::CommentHandler
{
  private:
    llvm::StringRef m_inFile;

  public:

    void set_InFile(llvm::StringRef inFile) 
    {
      m_inFile = inFile;
    }

    virtual bool HandleComment( clang::Preprocessor &pp, clang::SourceRange rng)
    {
      clang::SourceManager &sm = pp.getSourceManager();
      if( sm.getFilename(rng.getBegin()) == m_inFile)
      {
        std::pair<FileID, unsigned int> startLoc = sm.getDecomposedLoc(rng.getBegin());
        std::pair<FileID, unsigned int> endLoc = sm.getDecomposedLoc(rng.getEnd());

        llvm::StringRef fileData = sm.getBufferData(startLoc.first);

        std::cout << fileData.substr(startLoc.second, endLoc.second - startLoc.second).str();
        std::cout << std::endl;
      }
      return false;
    }
};

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
class MyFactory : public clang::ASTFrontendAction
{
  private:
    MyCommentHandler ch;

  public:
    clang::ASTConsumer *CreateASTConsumer(clang::CompilerInstance &ci, llvm::StringRef inFile) {
      ch.set_InFile(inFile);
      ci.getPreprocessor().addCommentHandler(&ch);
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
  ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());
  FrontendActionFactory *factory = newFrontendActionFactory<tooling::MyFactory>();
  Tool.run(factory);
  return 0;
}
