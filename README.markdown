# About #
This is a collection of tutorials showing off how to use core Clang types. It is based directly on two older tutorials which no longer built due to code rot.

1. [tutorial 1](http://amnoid.de/tmp/clangtut/tut.html) by Nico Weber - 9/28/2008
2. [tutorial 2](http://www.cs.rpi.edu/~laprej/clang.html) by Justin LaPre at Rensselaer Polytechnic Institute - 10/20/2009
3. [tutorial 3](https://github.com/loarabia/Clang-tutorial/wiki/TutorialOrig) by Larry Olson - 4/14/2012

This particular set of tutorials tracks the llvm / clang mainline and is updated semi-regularly to account for llvm / clang API changes.

See contents of the links above for a walkthrough of what these tutorials are doing.

# Last Update #
This was last built on 11/24/2013 against  
URL: http://llvm.org/svn/llvm-project/llvm/trunk  
Revision:195570

# Other Options #
The Clang team has been hard at work making it easier to write tools using Clang. There are [4 options](http://clang.llvm.org/docs/Tooling.html)
for developing tools using clang and llvm infrastructure.

# Latest Stable LLVM / Clang (v3.3) #
The master branch tracks recent commits to the clang and llvm svn. The tutorial assumes
you have grabbed a copy of both llvm and clang by following [these instructions](http://clang.llvm.org/get_started.html)
and that you have modified the makefile of this project to point to the build's resulting llvm-config. If you want
the latest public release, then checkout the *3.3* branch.

    git clone git@github.com:loarabia/Clang-tutorial.git
    git checkout 3.3

# CI tutorials #
The tutorials prefixed with CI are the same as the original tutorials but use the CompilerInstance object and
its helper methods to perform the same tasks as the original tutorials. For the most part, this makes the code
much more compact.

# Tooling tutorials #
These tutorials (and the clang tooling infrastrucutre) depend on having a file called `compile_commands.json` which
defines the commands used to compile the input file. You will need to modify this file's `directory` field to point
to the absolute path of the Clang-tutorial on your storage.

# Windows Build #
Note on the Windows build: Currently the paths are hardcoded. Please see the SharedBuild.targets file
inside of the SharedBuild project to update the path for your specific LLVM and CLANG install.

In particular, the LLVMLibsDirs property and the LLVMIncludes property should be updated.

# Contact Me #
For any questions, please ping me via my github account. Changes and additions are always welcome.
