/***   CIrewriter.cpp   ******************************************************
 * This code is licensed under the New BSD license.
 * See LICENSE.txt for details.
 *
 * This tutorial was written by Robert Ankeney.
 * Send comments to rrankene@gmail.com.
 * 
 * This tutorial is an example of using the Clang Rewriter class coupled
 * with the RecursiveASTVisitor class to parse and modify C code.
 *
 * Expressions of the form:
 *     (expr1 && expr2)
 * are rewritten as:
 *     L_AND(expr1, expr2)
 * and expressions of the form:
 *     (expr1 || expr2)
 * are rewritten as:
 *     L_OR(expr1, expr2)
 *
 * Functions are located and a comment is placed before and after the function.
 *
 * Statements of the type:
 *   if (expr)
 *      xxx;
 *   else
 *      yyy;
 *
 * are converted to:
 *   if (expr)
 *   {
 *      xxx;
 *   }
 *   else
 *   {
 *      yyy;
 *   }
 *
 * And similarly for while and for statements.
 *
 * Interesting information is printed on stderr.
 *
 * Usage:
 * CIrewriter <options> <file>.c
 * where <options> allow for parameters to be passed to the preprocessor
 * such as -DFOO to define FOO.
 *
 * Generated as output <file>_out.c
 *
 * Note: This tutorial uses the CompilerInstance object which has as one of
 * its purposes to create commonly used Clang types.
 *****************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <vector>

#include "llvm/Support/Host.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/FileSystem.h"

#include "clang/Basic/DiagnosticOptions.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Basic/TargetOptions.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Lex/Lexer.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/Parse/ParseAST.h"
#include "clang/Rewrite/Frontend/Rewriters.h"
#include "clang/Rewrite/Core/Rewriter.h"

using namespace clang;

// RecursiveASTVisitor is is the big-kahuna visitor that traverses
// everything in the AST.
class MyRecursiveASTVisitor
    : public RecursiveASTVisitor<MyRecursiveASTVisitor>
{

 public:
  MyRecursiveASTVisitor(Rewriter &R) : Rewrite(R) { }
  void InstrumentStmt(Stmt *s);
  bool VisitStmt(Stmt *s);
  bool VisitFunctionDecl(FunctionDecl *f);
  Expr *VisitBinaryOperator(BinaryOperator *op);

  Rewriter &Rewrite;
};

// Override Binary Operator expressions
Expr *MyRecursiveASTVisitor::VisitBinaryOperator(BinaryOperator *E)
{
  // Determine type of binary operator
  if (E->isLogicalOp())
  {
    // Insert function call at start of first expression.
    // Note getLocStart() should work as well as getExprLoc()
    Rewrite.InsertText(E->getLHS()->getExprLoc(),
             E->getOpcode() == BO_LAnd ? "L_AND(" : "L_OR(", true);

    // Replace operator ("||" or "&&") with ","
    Rewrite.ReplaceText(E->getOperatorLoc(), E->getOpcodeStr().size(), ",");

    // Insert closing paren at end of right-hand expression
    Rewrite.InsertTextAfterToken(E->getRHS()->getLocEnd(), ")");
  }
  else
  // Note isComparisonOp() is like isRelationalOp() but includes == and !=
  if (E->isRelationalOp())
  {
    llvm::errs() << "Relational Op " << E->getOpcodeStr() << "\n";
  }
  else
  // Handles == and != comparisons
  if (E->isEqualityOp())
  {
    llvm::errs() << "Equality Op " << E->getOpcodeStr() << "\n";
  }

  return E;
}

// InstrumentStmt - Add braces to line of code
void MyRecursiveASTVisitor::InstrumentStmt(Stmt *s)
{
  // Only perform if statement is not compound
  if (!isa<CompoundStmt>(s))
  {
    SourceLocation ST = s->getLocStart();

    // Insert opening brace.  Note the second true parameter to InsertText()
    // says to indent.  Sadly, it will indent to the line after the if, giving:
    // if (expr)
    //   {
    //   stmt;
    //   }
    Rewrite.InsertText(ST, "{\n", true, true);

    // Note Stmt::getLocEnd() returns the source location prior to the
    // token at the end of the line.  For instance, for:
    // var = 123;
    //      ^---- getLocEnd() points here.

    SourceLocation END = s->getLocEnd();

    // MeasureTokenLength gets us past the last token, and adding 1 gets
    // us past the ';'.
    int offset = Lexer::MeasureTokenLength(END,
                                           Rewrite.getSourceMgr(),
                                           Rewrite.getLangOpts()) + 1;

    SourceLocation END1 = END.getLocWithOffset(offset);
    Rewrite.InsertText(END1, "\n}", true, true);
  }

  // Also note getLocEnd() on a CompoundStmt points ahead of the '}'.
  // Use getLocEnd().getLocWithOffset(1) to point past it.
}

// Override Statements which includes expressions and more
bool MyRecursiveASTVisitor::VisitStmt(Stmt *s)
{
  if (isa<IfStmt>(s))
  {
    // Cast s to IfStmt to access the then and else clauses
    IfStmt *If = cast<IfStmt>(s);
    Stmt *TH = If->getThen();

    // Add braces if needed to then clause
    InstrumentStmt(TH);

    Stmt *EL = If->getElse();
    if (EL)
    {
      // Add braces if needed to else clause
      InstrumentStmt(EL);
    }
  }
  else
  if (isa<WhileStmt>(s))
  {
    WhileStmt *While = cast<WhileStmt>(s);
    Stmt *BODY = While->getBody();
    InstrumentStmt(BODY);
  }
  else
  if (isa<ForStmt>(s))
  {
    ForStmt *For = cast<ForStmt>(s);
    Stmt *BODY = For->getBody();
    InstrumentStmt(BODY);
  }

  return true; // returning false aborts the traversal
}

bool MyRecursiveASTVisitor::VisitFunctionDecl(FunctionDecl *f)
{
  if (f->hasBody())
  {
    SourceRange sr = f->getSourceRange();
    Stmt *s = f->getBody();

    // Make a stab at determining return type
    // Getting actual return type is trickier
    QualType q = f->getReturnType();
    const Type *typ = q.getTypePtr();

    std::string ret;
    if (typ->isVoidType())
       ret = "void";
    else
    if (typ->isIntegerType())
       ret = "integer";
    else
    if (typ->isCharType())
       ret = "char";
    else
       ret = "Other";

    // Get name of function
    DeclarationNameInfo dni = f->getNameInfo();
    DeclarationName dn = dni.getName();
    std::string fname = dn.getAsString();

    // Point to start of function declaration
    SourceLocation ST = sr.getBegin();

    // Add comment
    char fc[256];
    sprintf(fc, "// Begin function %s returning %s\n", fname.data(), ret.data());
    Rewrite.InsertText(ST, fc, true, true);

    if (f->isMain())
      llvm::errs() << "Found main()\n";

    SourceLocation END = s->getLocEnd().getLocWithOffset(1);
    sprintf(fc, "\n// End function %s\n", fname.data());
    Rewrite.InsertText(END, fc, true, true);
  }

  return true; // returning false aborts the traversal
}

class MyASTConsumer : public ASTConsumer
{
 public:

  MyASTConsumer(Rewriter &Rewrite) : rv(Rewrite) { }
  virtual bool HandleTopLevelDecl(DeclGroupRef d);

  MyRecursiveASTVisitor rv;
};


bool MyASTConsumer::HandleTopLevelDecl(DeclGroupRef d)
{
  typedef DeclGroupRef::iterator iter;

  for (iter b = d.begin(), e = d.end(); b != e; ++b)
  {
    rv.TraverseDecl(*b);
  }

  return true; // keep going
}


int main(int argc, char **argv)
{
  struct stat sb;

  if (argc < 2)
  {
     llvm::errs() << "Usage: CIrewriter <options> <filename>\n";
     return 1;
  }

  // Get filename
  std::string fileName(argv[argc - 1]);

  // Make sure it exists
  if (stat(fileName.c_str(), &sb) == -1)
  {
    perror(fileName.c_str());
    exit(EXIT_FAILURE);
  }

  CompilerInstance compiler;
  DiagnosticOptions diagnosticOptions;
  compiler.createDiagnostics();
  //compiler.createDiagnostics(argc, argv);

  // Create an invocation that passes any flags to preprocessor
  CompilerInvocation *Invocation = new CompilerInvocation;
  CompilerInvocation::CreateFromArgs(*Invocation, argv + 1, argv + argc,
                                      compiler.getDiagnostics());
  compiler.setInvocation(Invocation);

  // Set default target triple
  llvm::IntrusiveRefCntPtr<TargetOptions> pto( new TargetOptions());
  pto->Triple = llvm::sys::getDefaultTargetTriple();
  llvm::IntrusiveRefCntPtr<TargetInfo>
     pti(TargetInfo::CreateTargetInfo(compiler.getDiagnostics(),
                                      pto.getPtr()));
  compiler.setTarget(pti.getPtr());

  compiler.createFileManager();
  compiler.createSourceManager(compiler.getFileManager());

  HeaderSearchOptions &headerSearchOptions = compiler.getHeaderSearchOpts();

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
  // To see what include paths need to be here, try
  // clang -v -c test.c
  // or clang++ for C++ paths as used below:
  headerSearchOptions.AddPath("/usr/include/c++/4.6",
          clang::frontend::Angled,
          false,
          false);
  headerSearchOptions.AddPath("/usr/include/c++/4.6/i686-linux-gnu",
          clang::frontend::Angled,
          false,
          false);
  headerSearchOptions.AddPath("/usr/include/c++/4.6/backward",
          clang::frontend::Angled,
          false,
          false);
  headerSearchOptions.AddPath("/usr/local/include",
          clang::frontend::Angled,
          false,
          false);
  headerSearchOptions.AddPath("/usr/local/lib/clang/3.3/include",
          clang::frontend::Angled,
          false,
          false);
  headerSearchOptions.AddPath("/usr/include/i386-linux-gnu",
          clang::frontend::Angled,
          false,
          false);
  headerSearchOptions.AddPath("/usr/include",
          clang::frontend::Angled,
          false,
          false);
  // </Warning!!> -- End of Platform Specific Code


  // Allow C++ code to get rewritten
  LangOptions langOpts;
  langOpts.GNUMode = 1; 
  langOpts.CXXExceptions = 1; 
  langOpts.RTTI = 1; 
  langOpts.Bool = 1; 
  langOpts.CPlusPlus = 1; 
  Invocation->setLangDefaults(langOpts,
                              clang::IK_CXX,
                              clang::LangStandard::lang_cxx0x);

  compiler.createPreprocessor(clang::TU_Complete);
  compiler.getPreprocessorOpts().UsePredefines = false;

  compiler.createASTContext();

  // Initialize rewriter
  Rewriter Rewrite;
  Rewrite.setSourceMgr(compiler.getSourceManager(), compiler.getLangOpts());

  const FileEntry *pFile = compiler.getFileManager().getFile(fileName);
  compiler.getSourceManager().createMainFileID(pFile);
  compiler.getDiagnosticClient().BeginSourceFile(compiler.getLangOpts(),
                                                &compiler.getPreprocessor());

  MyASTConsumer astConsumer(Rewrite);

  // Convert <file>.c to <file_out>.c
  std::string outName (fileName);
  size_t ext = outName.rfind(".");
  if (ext == std::string::npos)
     ext = outName.length();
  outName.insert(ext, "_out");

  llvm::errs() << "Output to: " << outName << "\n";
  std::string OutErrorInfo;
  llvm::raw_fd_ostream outFile(outName.c_str(), OutErrorInfo, llvm::sys::fs::F_None);

  if (OutErrorInfo.empty())
  {
    // Parse the AST
    ParseAST(compiler.getPreprocessor(), &astConsumer, compiler.getASTContext());
    compiler.getDiagnosticClient().EndSourceFile();

    // Output some #ifdefs
    outFile << "#define L_AND(a, b) a && b\n";
    outFile << "#define L_OR(a, b) a || b\n\n";

    // Now output rewritten source code
    const RewriteBuffer *RewriteBuf =
      Rewrite.getRewriteBufferFor(compiler.getSourceManager().getMainFileID());
    outFile << std::string(RewriteBuf->begin(), RewriteBuf->end());
  }
  else
  {
    llvm::errs() << "Cannot open " << outName << " for writing\n";
  }

  outFile.close();

  return 0;
}

