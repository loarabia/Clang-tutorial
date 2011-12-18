#include <string>

#include "Indenter.h"

using std::string;

Indenter::Indenter(string inputFilename) : InputFile(inputFilename)
{
    InputFile.createTemporaryFileOnDisk();
}
