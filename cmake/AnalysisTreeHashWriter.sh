#!/bin/bash

FILE_HASH=AnalysisTreeHash.sh
FILE_DIFF=AnalysisTreeDiff.patch
if [ -f $FILE_HASH ]; then
rm $FILE_HASH
fi

SRC_DIR=${1}

cd $SRC_DIR
if [ -d ".git" ]; then
GITTAG=$(git describe --tags)
GITCOMMIT=$(git rev-parse HEAD)
GITSTATUS=$(git status --porcelain)
cd -
echo "export ANALYSIS_TREE_TAG=\"${GITTAG}\"" >> $FILE_HASH
echo "export ANALYSIS_TREE_COMMIT_HASH=${GITCOMMIT}" >> $FILE_HASH
if [ -z "${GITSTATUS}" ]; then
echo "export ANALYSIS_TREE_COMMIT_ORIGINAL=TRUE" >> $FILE_HASH
else
echo "export ANALYSIS_TREE_COMMIT_ORIGINAL=FALSE" >> $FILE_HASH
git diff >> $FILE_DIFF
fi
else
cd -
echo "export ANALYSIS_TREE_TAG=NOT_A_GIT_REPO" >> $FILE_HASH
echo "export ANALYSIS_TREE_COMMIT_HASH=NOT_A_GIT_REPO" >> $FILE_HASH
echo "export ANALYSIS_TREE_COMMIT_ORIGINAL=NOT_A_GIT_REPO" >> $FILE_HASH
fi
