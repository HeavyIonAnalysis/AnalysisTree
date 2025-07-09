# AnalysisTree
![Workflow status](https://github.com/HeavyIonAnalysis/AnalysisTree/workflows/CMake/badge.svg)
![GitHub tag (latest SemVer)](https://img.shields.io/github/v/tag/HeavyIonAnalysis/AnalysisTree?sort=semver)
![GitHub release (latest SemVer)](https://img.shields.io/github/v/release/HeavyIonAnalysis/AnalysisTree)

## Important note on AnalysisTree versions

**Current version is development version!**

Last tested version: v2.3.2

Last release version: v.2.3.2

Different MAJOR version means incompatible API changes. 
Neither API nor data compatibility is preserved between MAJOR versions.

Since v2.2.1 old infra implementation is kept for the sake of code compatibility until the next MAJOR infra release.

## Doxygen documentation:

https://heavyionanalysis.github.io/AnalysisTree/

## General information

This is a dataformat to store information in configurable objects.
Basic objects are:<br>
 - Track (momentum)<br>
 - Particle (momentum and PID)<br>
 - Hit (position and signal)<br>
 - Module (id and signal)<br>
Four mentioned objects are so-called channelized:
they contain set of channels (namely tracks, particles, hits or modules) in each event<br>
 - EventHeader (vertex position) - contains information related to specific event<br>
 - DataHeader - contains common information for all events<br>
 - Matching - establishes a correspondence between channels in channelized objects (e.g. between simulated particle and reconstructed track).

Additionally to mandatory information specified in round brackets (a.k.a. "default fields"), each object can contain any number of integer, floating or boolean fields (a.k.a. "user-defined fields").
Information about all fields in all branches is stored in Configuration object.

## Installation

ROOT6 is needed for installation.
Follow CERN ROOT [instructions](https://root.cern/install/) to install it.
Version compiled with c++17 flag is preferred, otherwise CMAKE_CXX_STANDARD flag needs to be explicitly specified (see below).

    git clone https://github.com/HeavyIonAnalysis/AnalysisTree.git
    cd AnalysisTree
    mkdir build install
    cd build
    source path-to-root-installation/bin/thisroot.sh
    cmake -DCMAKE_INSTALL_PREFIX=../install ../
    make -j install

*path-to-root-installation* must be replaced with your actual location of Root install directory.
  
### List of CMake options:
To apply the flag use -D{Name}={value}, for example, if you want to compile using c++11:
    
    cmake -DCMAKE_CXX_STANDARD=11 ../

| Name  | Default value | Possible values |
| ------------- | ------------- | ---------- |
| CMAKE_BUILD_TYPE  | RELEASE  | RELEASE/DEBUG |
| CMAKE_CXX_STANDARD  | 17  | 11/14/17 |
| AnalysisTree_BUILD_TESTS  | OFF  | ON/OFF |
| AnalysisTree_BUILD_EXAMPLES  | OFF  | ON/OFF |
| AnalysisTree_BUILD_INFRA_1  | OFF  | ON/OFF |

## Operating with AnalysisTree
### Setting AnalysisTree environment
Whatever you are going to do with AnalysisTree - read the file, perform analysis based on information stored in it or create your own file, first of all you need to set up environment variables.
It can be done in a single command:

    source path-to-analysis_tree-installation/bin/AnalysisTreeConfig.sh

### Reading files from ROOT session
An example of AnalysisTree ROOT file can be downloaded by this [link](https://sf.gsi.de/f/3ba5a9e3ff5248edba2c/?dl=1)

Open a ROOT-file

    root -l 1.analysistree.root

Check its content

    .ls

You will obtain something like this:

    TFile**		1.analysistree.root
     TFile*		1.analysistree.root
      KEY: TTree	rTree;1	AnalysisTree
      KEY: AnalysisTree::Configuration	Configuration;1
      KEY: AnalysisTree::DataHeader	DataHeader;1

So, file contains ROOT::TTree object named *rTree*,
AnalysisTree::Configuration object named *Configuration* describing the structure and hierarchy of the tree and thus needed for proper reading the tree
and, finally, AnalysisTree::DataHeader object named *DataHeader* containing information common for all events.

#### Reading the configuration
In order to know the structure of the tree, perform following command:

    Configuration->Print()

An output will contain plenty of branches and matchings between them.
Let us look at one of them:

![](https://github.com/HeavyIonAnalysis/AnalysisTree/blob/master/docs/pics/SimParticles.png)

There are listed integer, floating and boolean fields of this branch
(boolean are empty in this particular case).
In the left column there is a unique id of the field.
Pay attention that the numeration of fields is separate for integers, floats and bools.
Negative ids belong to default fields of branches while positive ids and 0 - to user-defined fields.
Middle column contains string name of the field, and right column - a description of it.

#### Digesting the tree content

    Configuration->GetBranchConfig("SimParticles").GetType()
    // to know to which type (Hit, Track, Module, Particle or EventHeader belongs SimParticles branch)

    rTree->Draw("SimParticles.channels_.GetPx()")
    // draw distribution of the value contained in the default field (has its own getter, namely
    // GetPx(), names of getters can be found in doxygen documentation)

    rTree->Draw("SimParticles.channels_.GetField<int>(2)")
    // draw distribution of the value contained in the user-defined field (specify int, float or bool
    // in angular brackets and field's id as an argument in round brackets)

    // Note that access to default fields is also possible via id, not only via getters:
    rTree->Draw("SimParticles.channels_.GetField<float>(-7)")

    // If the branch contains non-channelized objects, but is an EventHeader, the word "channels_"
    // is not needed in the command:
    rTree->Draw("SimEventHeader.GetVertexY()")
    rTree->Draw("RecEventHeader.GetField<int>(0)")

    // The Draw() method allows usage of ROOT::TTree-specific drawing features such as:

    rTree->Draw("SimParticles.channels_.GetPx() : SimParticles.channels_.GetPy()")
    // drawing a 2D histogram, but only for fields from the same branch.

    rTree->Draw("SimParticles.channels_.GetPx()", "SimParticles.channels_.GetPid()==211")
    // drawing with some additional condition (px for positive pions only)

    rTree->Draw("SimParticles.channels_.GetPx()", "", "same")
    // drawing with user-specified draw options

    rTree->Draw("SimParticles.channels_.GetPx()>>hist(1000,-10,10)")
    // drawing a histogram with user-defined binning and ranges.

    rTree->Draw("TMath::Log(TMath::Abs(SimParticles.channels_.GetPx()))")
    // drawing a distribution of derived quantites calculated by formula

Moreover, for default fields which are explicitly present in Container (i.e. px is OK, but not pt, which is not stored but calculated on fly) there is a possibility to draw them using TTree::Draw syntax:

    rTree->Draw("SimParticles.px_")
    rTree->Draw("TMath::Log(TMath::Abs(SimParticles.px_))")
    rTree->Draw("TMath::Log(TMath::Abs(SimParticles.px_)) * TMath::Cos(SimParticles.py_)")

Also you can open a ROOT interactive session and create an AnalysisTree::Chain:

    AnalysisTree::Chain t("1.analysistree.root", "rTree") // Chain constructor with a single file
    // or
    AnalysisTree::Chain t({"filelist.txt"}, {"rTree"})  // Chain constructor with a file list

and then build any fields including user-defined and implicitly present fields (such as phi or pt):

    t.Draw("SimParticles.px")
    t.Draw("SimParticles.pid")
    t.Draw("VtxTracks.chi2")
    // 2D histograms, cuts, drawing options and math formulas mentioned above are also available

### Reading file with a macro

Building of distributions in interactive mode is a good approach only if the commands are simple, and the number of events to be analized is not so big.
However it is not common to do it interactively if formulae you want to use are complicated, and you can easely introduce a typo.
Or if your logic includes a matching between branches - then interactive building distributions is totally impossible.
For this case a good solution is usage of macros.
A comprehensive example of reading the AnalysisTree ROOT file is shown in [examples/ReadingMacro.C](https://github.com/HeavyIonAnalysis/AnalysisTree/blob/master/examples/ReadingMacro.C).

### Creating your own AnalysisTree file

A comprehensive example of creating the AnalysisTree ROOT file is shown in [examples/WritingMacro.C](https://github.com/HeavyIonAnalysis/AnalysisTree/blob/master/examples/WritingMacro.C).

### Reading and writing with executable

Usage of compiled code instead of macros has some advantachges, in first turn computation speed, and also syntax check in compile time.
Macros can be re-qritten into executables.
For this purpose one needs:

    1. Put necessary headers with #include command
    2. Create an int main() function - an entry point of any C++ program
    3. Add an executable into CMakeLists

An example how to do it is shown with [examples/UserTaskRead.cpp](https://github.com/HeavyIonAnalysis/AnalysisTree/blob/master/examples/UserTaskRead.cpp) and [examples/UserTaskWrite.cpp](https://github.com/HeavyIonAnalysis/AnalysisTree/blob/master/examples/UserTaskWrite.cpp).

Note: these examples use a little bit different syntax from one shown in examples above.
It is a so-called syntax sugar - attempt to make syntax more user-friendly.
However it has its own price in terms of performance - it is slower by factor of few units than one shown above.

### QA of files
For a QA of the file(s) please use the dedicated package AnalysisTreeQA:<br>
https://github.com/HeavyIonAnalysis/AnalysisTreeQA

## Known problems (features)
 - BranchConfig::GetFieldId() is slow -> should not be used for every event/track/hit/module
 - In some systems (e.g.  at MacOS) there is a linking problem when using AnalysisTree::Infra
 objects in interactive mode or in macros. However it is easely fixable by silent call an
 AnalysisTree::Base object, e.g. AnalysisTree::Hit() - without doing nothing with it, just call.
