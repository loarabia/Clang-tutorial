/******************************************************************************
 * Copyright Larry Olson
 * Source is Licensed under the NewBSD License, See ../LICENSE.txt
 *
 * If there is anything you need the OS to do before you parse args and run
 * the tool, set it up and tear it down here.
 *****************************************************************************/
#include "llvm/Support/CommandLine.h"

#include "OptionParser.h"
#include "Indenter.h"

/******************************************************************************
 * Main parses the args, instantiates an indenter object, and runs the indenter
 * object.
 *****************************************************************************/
int main(int argc, char **argv)
{
    using namespace llvm;

    cl::ParseCommandLineOptions(argc, argv); 
    Indenter indenter(OptionParser::InputFilename);

    return 0;
}
