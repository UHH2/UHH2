# UHH2 framework

This framework is for data analysis in the UHH group for the 13TeV era.
It replaces the `UHHAnalysis` code (see https://github.com/UHHAnalysis/) and
uses (partially) the same data format.

## Overview

The central class in this system is `AnalysisModule` (it can
be thought of as a replacement of `UHHAnalysis`' `AnalysisCycle`). The data format
is compatible with the `UHHAnalysis` one (that, however, might change to
include variables needed only at 13TeV). A good starting point are the `examples` to see how to
implement an analysis. Note that it is possible to run an `AnalysisModule` in two different ways unchanged:
Standalone using SFrame and as part of the `NtupleWriter` in a `cmsRun` job. This double use enables new workflows.



## Installation

### Install SFrame

As first step, install SFrame. To start with a clean setup, it is recommended to install a new version of SFrame via

```
svn co https://svn.code.sf.net/p/sframe/code/SFrame/tags/SFrame-04-00-01/ SFramePhys14
```

Before compiling SFrame, remember to setup CMSSW first and source `setup.sh` in the SFrame directory (see below in compiling).


### Install `UHH2`

To install `UHH2`, clone it with git into an existing CMSSW installation, e.g.:

```
cmsrel CMSSW_7_2_1_patch4
cd CMSSW_7_2_1_patch4/src
git clone https://github.com/jottCern/UHH2.git
```

### Compiling and Environment

After the installation, compile the SFrame core by executing `make` in the directory you used to checkout SFrame into. This has only to be done once.

To compile `UHH2` code, change to the `UHH2` directory and execute `make`. If you also want to run the CMSSW part (i.e. `NtupleWriter` or execute `AnalysisModules`
within CMSSW), you also have to run `scram b`. For cleaning up, use `make clean` or `scram b clean` / `scram b distclean`. Cleaning up manually can be done by removing
all files in the `obj` subdirectory (which is the location used by all auto-generated files) and the SFrame libraries, i.e. `$SFRAME_DIR/lib/libSUHH2*`.

Concerning the environment, it is important to make sure you always use a consistent setup when compiling and running the code.
A good thing to do is to always first set up CMSSW, then set up sframe, i.e. execute an analogue of these commands for every new shell:

```
cd CMSSW_7_2_1_patch4
cmsenv
cd ~/SFramePhys14
source setup.sh
```

**Note:** If you have an existing SFrame setup, you might have your own installation of fastjet. This can be a problem when trying to run CMSSW, as CMSSW also
depends on fastjet, but has some specific plugins. Therefore, it's best to not install fastjet or setup your environment such that the fastjet library of CMSSW is used
when running `cmsRun`.


## Code structure and conventions

The subdirectories of `UHH2` contain:
 * `core`: ntuple data format for dictionaries; crucial abstract base classes (such as `AnalysisModule`, `Selection`, ...); glue code to SFrame (`AnalysisModuleRunner`) and for CMSSW (`NtupleWriter` defined in  `plugins`)
 * `common`: object ids and event selection modules common to several analyses
 * `common/datasets` and subdirectories: SFrame xml files defining input data files
 * `examples`: example implementation of an `AnalysisModule`, including SFrame configuration
 * `scripts`: non-C++ code/scripts and other files (such as configuration files) required in various steps in the workflow; right now mainly a grid-control configuration example for running `NtupleWriter` at the NAF

All subdirectories containing code have the include files in `include` and the C++ source code in `src`. A special
case is `NtupleWriter` which is the only package also providing code to be compiled within CMSSW only; it is located in `plugins`.

Most of the code structure is a result of the desire to be able to build the same code either within CMSSW via `scram` and also
to build it for SFrame via `make`. Therefore, please stick to the structure for new code, unless you want to do something special (such as
creating code build only in CMSSW or only in SFrame, which should be rarely necessary).

You might notice that libraries in SFrame follow a slightly different naming convention than those in CMSSW: The library built from
`UHH2/core` is called `libSUHH2core.so` in SFrame (see the `LIBRARY` variable in `core/Makefile`, which controls the library name). In CMSSW, however, it is called `libUHH2core.so`, following
the conventions of CMSSW. Please follow the same library naming conventions when setting `LIBRARY` in your new subdirectory's `Makefile`; the reason for having `SUHH2`
instead of `UHH2` in the SFrame library names is to avoid name clashes of the created shared object files between the SFrame and CMSSW.
 

## Event I/O, workflow

`AnalysisModules` can run in both CMSSW and standalone SFrame, but some important restrictions exist
concerning the event content:
 * jet constituent information is only available when running in CMSSW, in order to save space for the SFrame ntuple format
 * A SFrame ntuple output file (e.g. selected events) can be created by both CMSSW and SFrame; in both cases only events for which the top-level `AnalysisModule::process` method returns `true` are saved in the output file. In both cases, it is possible for an `AnalysisModule` to define additional event output via `Context::declare_event_output`.
 * For CMSSW, it is also possible to write a CMSSW output file (in addition or instead of saving the SFrame ntuple). However, only CMSSW-level content can be saved (such as miniAOD products), i.e. and no additional event data created by `AnalysisModules` is saved.
   
This suggests the following workflow: In a first step, run CMSSW to run all special clustering algorithms and write a CMSSW output (an 'extended miniAOD' with some additional jet collections). The same cmsRun job also produces SFrame ntuples. In a second step, analyses can either run standalone SFrame on the ntuples or (if they study new top taggers)
run within CMSSW on miniAOD directly, which would typically mean to run some additional CMSSW modules before running the NtupleWriter.

An implementation of the first step is available as grid-control configuration in `scripts/gc-conf`.

## Miscellaneous Information

### Event sizes

 * plain miniAOD: 40-60kB/event, depending on sample and pileup scenario
 * old SFrame ntuple (with all jet constituents): around 10% less than plain miniAOD
 * miniAOD+topjets: around 80kB/event for high-pileup --> adds around 30% to plain miniAOD (main culprit: patJetCA8CHS, so could be trimmed down a little)
 * small version of SFrame ntuple (=without jet constituents): around 5kB/event


### Typical performance

 * running ntuplewriter only (on miniAOD+topjets sample), writing the small ntuple: 300 events/sec. This is the "baseline performance" to be expected when running AnalysisModules within CMSSW.
 * running ntuplewriter only, but writing large ntuple: 6 events/sec
 * running on miniAOD and writing both, miniAOD+topjets file AND small ntuple: 2 events/sec (for large ntuple: 1.5 events/s)
 
 
## Design Choices

The `AnalysisModule` is independent of the SFrame class hierarchy to allow more flexible use, such
as running in a CMSSW job or (as a future possibility) to run it within another
framework. For the same reason, the number of methods for `AnalysisModule` and
for Context is very small; there is no BeginCycle / EndCycle method and similar methods.
Basically only two methods are kept: The role of BeginInputData is now the
constructor (all AnalysisModules are re-created for each dataset), and the
ExecuteEvent method corresponds to the 'process' method. Having no EndCycle, 
EndInputData, etc. methods is on purpose: they hardly had any legitimate use,
e.g. using them to divide histograms to get fractions/efficiencies is not a good idea,
as this does not work in distributed (PROOF) mode, in which all the divided histograms
would be added again. Also, `AnalysisCycle` provided methods which are only
called on the worker or only on the master, if in PROOF mode. These are also
removed on purpose, as they expose an implementation detail (the distributed execution),
which should remain abstracted away: The user having to know whether or not
the dataset is processed in PROOF or not is a sign of bad design choices.
So limiting the ways of how the `AnalysisModules` can interact with the framework to
the bare minimum increases both flexibility and robustness and decreases the
likelihood of errors.
