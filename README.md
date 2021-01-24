# AnalysisTree
[![Build Status](https://travis-ci.com/HeavyIonAnalysis/AnalysisTree.svg?branch=master)](https://travis-ci.com/HeavyIonAnalysis/AnalysisTree)
![GitHub tag (latest SemVer)](https://img.shields.io/github/v/tag/HeavyIonAnalysis/AnalysisTree?sort=semver)
![GitHub release (latest by date including pre-releases)](https://img.shields.io/github/v/release/HeavyIonAnalysis/AnalysisTree?include_prereleases)
## Doxygen documentation:

https://heavyionanalysis.github.io/AnalysisTree/

## General information

This is a dataformat to store information in configurable objects. Basic objects are:
 - Track (momentum)
 - Particle (momentum and PID)
 - Hit (position and signal)
 - Module (id and signal)
 - EventHeader (vertex position)
 - DataHeader (common information for all events)

Additionaly to mandatory information, each object can contain any number of integer, floating or boolean fields.
Information about all fields in all branches is stored in Configuration object.

## Installation

ROOT6 is needed for installation. Version compiled with c++17 flag is preferred, otherwise CMAKE_CXX_STANDARD flag needs to be implicitly specified (see below).

    git clone https://github.com/HeavyIonAnalysis/AnalysisTree.git
    cd AnalysisTree
    mkdir build install
    cd build
    source /path/to/your/thisroot.sh
    cmake ../
    make -j install
  
### List of CMake options:

To apply the flag use -D{Name}={value}, for example, if you want to compile using c++11:
    
    cmake -DCMAKE_CXX_STANDARD=11 ../

| Name  | Default value | Possible values |
| ------------- | ------------- | ---------- |
| CMAKE_BUILD_TYPE  | RELEASE  | RELEASE/DEBUG |
| CMAKE_CXX_STANDARD  | 17  | 11/14/17 |
| AnalysisTree_BUILD_TESTS  | OFF  | ON/OFF |
| AnalysisTree_BUILD_EXAMPLES  | ON  | ON/OFF |
| AnalysisTree_BUILD_EXPERIMENTAL  | OFF  | ON/OFF |

## Reading AnalysisTree 

### Load AnalysisTree libraries

First of all you need to load AnalysisTree libraries into your system. Usually, the path to libraries is added to LD_LIBRARY_PATH:

    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/path/to/AnalysisTree/install/lib/

After that one can open ROOT session and load libraries:

    root -l
    gSystem->Load("libAnalysisTreeBase.so")

### Read file

Open a ROOT-file and check its content:
    
    root -l filename.root
    [0] gSystem->Load("libAnalysisTreeBase.so")
    [1] .ls
    
You should see something like:

    aTree;
    Configuration;
    DataHeader;
    
Print the information about AnalysisTree structure:

    Configuration->Print()

You should see something like:
     blahblahblah

Draw any field you like:

    aTree->Draw("BranchName.FieldName")

All standart TTree::Draw operations are supported.

For a QA of the file(s) please use the dedicated package AnalysisTreeQA:

https://git.cbm.gsi.de/pwg-c2f/data/analysis_tree_qa/

## Cuts

Coming soon =)

## Creating your own AnalysisTree 

Coming soon =)

## Known problems (features)
 - BranchConfig::GetFieldId() is slow -> should not be used for every event/track/hit/module
 - Branch name and title of the corresponding object should be the same
 - For every new (Container-based) object Init(BranchConfig) should be called to initialize internal structure 
