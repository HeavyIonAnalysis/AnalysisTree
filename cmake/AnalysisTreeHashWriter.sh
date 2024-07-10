#!/bin/bash

FILE=AnalysisTreeHash.sh
if [ -f $FILE ]; then
rm $FILE
fi

SRC_DIR=${1}

cd $SRC_DIR
if [ -d ".git" ]; then
GITCOMMIT=$(git rev-parse HEAD)
GITSTATUS=$(git status --porcelain)
cd -
echo "export ANALYSIS_TREE_COMMIT_HASH=${GITCOMMIT}" >> $FILE
if [ -z "${GITSTATUS}" ]; then
echo "export ANALYSIS_TREE_COMMIT_ORIGINAL=TRUE" >> $FILE
else
echo "export ANALYSIS_TREE_COMMIT_ORIGINAL=FALSE" >> $FILE
fi
else
cd -
echo "export ANALYSIS_TREE_COMMIT_HASH=NOT_A_GIT_REPO" >> $FILE
echo "export ANALYSIS_TREE_COMMIT_ORIGINAL=NOT_A_GIT_REPO" >> $FILE
fi
