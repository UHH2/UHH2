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



## Code organization, installation

To install `UHH2`, clone it with git into an existing CMSSW installation, e.g.:

```
cmsrel CMSSW_7_2_1_patch4
cd CMSSW_7_2_1_patch4/src
git clone https://github.com/jottCern/UHH2.git
```

To compile the code for CMSSW, run `scram b` as usual.

The code is in directories with two directory hierarchies (`UHH2/*`) to have the same directory structure
as expected by CMSSW. This allows to compile the same code within SFrame and within CMSSW unchanged. To 'install'
the code in SFrame the best choice is to make a symlink to the CMSSW installation (it is assumed that you have
a SFrame installation set up).

```
cd $SFRAME_DIR
ln -s $CMSSW_BASE/src/UHH2 .
```

Compiling code for SFrame is done by executing `make` in the `UHH2` directory; this will build all the subdirectories.

The subdirectories of `UHH2` contain:
 * `core`: ntuple data format for dictionaries; crucial abstract base classes (such as `AnalysisModule`, `Selection`, ...); glue code to SFrame (`AnalysisModuleRunner`) and for CMSSW (`NtupleWriter` defined in  `plugins`)
 * `common`: object ids and event selection modules common to several analyses
 * `common/datasets` and subdirectories: SFrame xml files defining input data files
 * `examples`: example implementation of an `AnalysisModule`, including SFrame configuration
 * `scripts`: scripts and/or configuration files required by various steps in the workflow, such as grid-control configuration

All subdirectories containing code have the include files in `include` and the C++ source code in `src`. A special
case is `NtupleWriter` which is the only package also providing code to be compiled within CMSSW only; it is located in `plugins`.
 

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
