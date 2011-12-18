/******************************************************************************
 * Copyright Larry Olson
 * Source is Licensed under the NewBSD License, See ../LICENSE.txt
 *
 * If there is anything you need the OS to do before you parse args and run
 * the tool, set it up and tear it down here.
 *****************************************************************************/
#include "llvm/Support/CommandLine.h"
#include "OptionParser.h"

/******************************************************************************
 * Main parses the args and runs the indenter and sets up or tears down 
 * anything as required by the OS.
 *****************************************************************************/
int main(int argc, char **argv)
{
    using namespace llvm;

    cl::ParseCommandLineOptions(argc, argv); 

    return 0;
}
