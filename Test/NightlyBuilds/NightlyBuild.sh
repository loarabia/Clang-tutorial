#!/bin/bash

# This code is licensed under the New BSD license.
# See LICENSE.txt for details

LLVM_BUILD_DIR=~/Code/build_llvm
TUT_DIR=~/Code/Clang-tutorial/

make -C $LLVM_BUILD_DIR happiness
if [ $? != 0 ]; then
    ./MailResults.py "FAIL: LLVM/Clang Build" 
    exit $?
fi

make -C $LLVM_BUILD_DIR install
if [ $? != 0 ]; then
    ./MailResults.py "FAIL: Couldn't Install LLVM/Clang"
    exit $?
fi

git pull $TUT_DIR
if [ $? != 0 ]; then
    ./MailResults.py "FAIL: Couldn't update Clang-tutorial"
    echo "PULL FAILED"
    exit $?
fi

make -C $TUT_DIR
if [ $? != 0 ]; then
    ./MailResults.py "FAIL: Couldn;t build Clang-tutorial "
    exit $?
fi

./MailResults.py "SUCCESS: Everything up-to-date"
