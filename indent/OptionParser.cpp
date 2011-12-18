/******************************************************************************
 * Copyright Larry Olson
 * Source is Licensed under the NewBSD License, See ../LICENSE.txt
 *****************************************************************************/
#include "OptionParser.h"

string OptionParser::InputFilename;

/******************************************************************************
 * Required Command Line Argument: InputFile 
 *  This file will be read, parsed, and rewritten.
 *
 * Note: The BSD and GNU Indent tool can also read in from stdin. Because of
 * the below declation, this one cannot.
 *****************************************************************************/
cl::opt<string, true> InputFile(cl::Positional,
                                cl::desc("<input file>"), 
                                cl::Required,
                                cl::location(OptionParser::InputFilename));
