#!/usr/bin/env python


"""Script to plot all variables of all collections a ntuple,
or compare them with another ntuple.
Methods are called recursively on objects.

JSON files with the summary info on plots and classes are produced.
"""


from __future__ import print_function

import os
import re
import json
import argparse
import inspect
from array import array
from operator import methodcaller, attrgetter
from itertools import chain
from collections import OrderedDict
from tqdm import tqdm
import ROOT


ROOT.PyConfig.IgnoreCommandLineOptions = True
ROOT.gROOT.SetBatch(1)
ROOT.TH1.SetDefaultSumw2()
# ROOT.gErrorIgnoreLevel = ROOT.kError
ROOT.gErrorIgnoreLevel = ROOT.kBreak  # to turn off all Error in <TCanvas::Range> etc


BUILTIN_TYPES = [
    # ROOT-isms, see TTree docs
    "C", "B", "b", "S", "s", "I", "i", "F", "D", "L", "l", "O",
    "Char_t", "UChar_t", "Short_t", "UShort_t", "Int_t", "UInt_t", "Float_t", "Double_t", "Long64_t", "ULong64_t", "Bool_t",
    # C++ ones
    "char", "unsigned char", "short", "unsigned short", "int", "unsigned int", "float", "double", "long", "unsigned long", "bool", "string"
]

# Methods specific to TLorentzVEcotr, ROOT::TMath::LorentzVector, since we don't
# want every method
LORENTZVECTOR_METHODS = ["E", "Pt", "Pz", "X", "Y", "Z"]

# don't use kBlack as you can't see it against the axes
HIST_COLOURS = [ROOT.kBlue, ROOT.kRed]


class BranchInfo(object):

    def __init__(self, name, classname, typename, title):
        self.name = name
        self.classname = classname
        self.typename = typename
        self.title = title
        if classname == "" and title != "":
            self.classname = title.split('/')[-1].strip()

    def __repr__(self):
        return ("{name}: {classname}, {typename}, {title}".format(**self.__dict__))

    def __str__(self):
        return ("{name}: {classname}, {typename}, {title}".format(**self.__dict__))

    def __eq__(self, other):
        return self.name == other.name and self.classname == other.classname


def store_branches(tree, obj_list, do_recursive=False, indent=0):
    """Iterate through tree (optionally recursively),
    storing branch info as BranchInfo objects to obj_list.

    Parameters
    ----------
    tree : TTree, TBranch
        Anything that has branches
    obj_list : list[BranchInfo]
        List to append BranchInfo objects to
    do_recursive : bool, optional
        If True, go through branches recursively
    indent : int, optional
        Indentation for printout. If None, does not print
    """
    for b in tree.GetListOfBranches():
        this_name = b.GetName()
        classname = b.GetClassName()
        typename = classname
        if isinstance(b, ROOT.TBranchElement):
            typename = b.GetTypeName()
        this_br_info = BranchInfo(name=this_name,
                                  classname=classname,
                                  typename=typename,
                                  title=b.GetTitle())
        new_indent = indent
        if indent is not None:
            print(" " * indent, this_br_info)
            new_indent += 2
        obj_list.append(this_br_info)
        if do_recursive:
            store_branches(b, obj_list, True, new_indent)


def parse_function_signature(signature):
    """Handle cpp function signature, giving return type & function name

    Parameters
    ----------
    signature : str
        C++ function signature

    Returns
    -------
    str, str, str
        Return type, function name, arguments
    """
    # remove bits we don't care about
    signature = signature.strip().replace("&", "").replace("*", "")
    signature = signature.replace("const", "").replace("final", "").replace("static", "")

    # remove extra spaces around < > ( )
    signature = re.sub(r'([(<]) *', r'\1', signature)
    signature = re.sub(r' +([>)])', r'\1', signature)

    # parts should now be of the form <return type> <class::method>
    # but need to be able to handle:
    # ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiE4D<Double32_t> > MET::uncorr_v4()
    # void GenTopJet::add_subjet( Particle p)
    # unsigned int PrimaryVertex::nTracks()
    # GenParticle GenParticle::daughter(vector<GenParticle> gplist, int ind = 1)

    # Form is: <return type> <class>::<method name>(<optional arg type>)
    m = re.match(r'([\w <>::,]+) ([\w <>::]+)::([\w <>::,]+)\(([\w <>:: =,]*)\)', signature)
    if m:
        return_type = m.group(1).strip()
        method_name = m.group(3).strip()
        args = m.group(4)
    else:
        raise RuntimeError("Couldn't parse signature %s" % signature)

    return return_type, method_name, args


def get_function_info(classname, method):
    """Get the return type for a given method in class `classname`

    Does manual override of return type for select classes to avoid issues later.

    Parameters
    ----------
    classname : str
        Name of C++ class
    method : str
        Method name (without the ())

    Returns
    -------
    str, str, str
        Return typename, method name, arguments
    """
    # Manually override these classes
    # TODO must be a better way than this
    if (("TLorentzVector" in classname or "Math.LorentzVector" in classname)
        and method in LORENTZVECTOR_METHODS):
        # Assumes every method returns a float!
        return "float", method, ""

    this_class = getattr(ROOT, classname)
    signature = getattr(this_class, method).__doc__  # get c++ method definition
    return_type, method_name, args = parse_function_signature(signature)
    return return_type, method_name, args


def get_class_getters_info(classname, class_infos, do_recursive=False):
    """For a given class, get all getter methods & their return types,
    and all the class' properties. Updates/adds class_infos entry for that classname.

    Parameters
    ----------
    classname : str
        Name of class
    class_infos : list
        Dict of getters/properties for each class
    do_recursive : bool, optional
        If True, build chained methods using getters from objects
        returned by this class' getters
    """
    getter_info = {}
    # print("Getting getters for", classname)

    # Get getter methods
    for method in get_class_methods(classname):
        if (method.startswith("set_")  # skip setters
            or "operator" in method    # skip operators for now
            or method in ["Class"]):   # ROOT added or special methods
            continue

        return_type, method_name, args = get_function_info(classname, method)
        # ensure we only have getters
        if return_type is None or return_type == "void" or return_type.startswith("_") or args != "":
            continue

        method_name += "()"  # need to store the () to distinguish from properties
        # make python-friendly so it can be used in getattr
        return_type = return_type.replace("::", ".")
        getter_info[method_name] = return_type

        if (do_recursive and
            ("." in return_type
             or unvectorise_classname(return_type) not in BUILTIN_TYPES
             and return_type != "void")):
            # Need to check if this return class is in our dict
            return_type = unvectorise_classname(return_type)
            # if return_type not in class_infos:
            get_class_getters_info(return_type, class_infos, do_recursive)

    # Get properties e.g. enums
    # FIXME is it possible to get return type? normally int or float, but how to check?
    # __doc__ doesn't work, neither dir(), nor inspect
    properties = [m for m in get_class_properties(classname)
                    if not (m.startswith("set_")  # skip setters
                            or "operator" in m    # skip operators for now
                            or m in ["Class"])    # ROOT added methods we don't want
                 ]

    class_infos[classname] = {"methods": getter_info, "properties": properties}
    # print(class_infos[classname])


def unvectorise_classname(classname):
    """Find the type of element stored in vector. Assumes form "vector<xxxx>".
    If no vector<, returns original class name

    Parameters
    ----------
    classname : str
        Name of class

    Returns
    -------
    str
    """
    classname = classname.strip()
    if classname.startswith("vector<"):  # use startswith to ensure not LorentzVector
        # hope there are no newlines...
        matches = re.search(r'vector<(.+)>', classname)
        if matches:
            classname = matches.group(1).strip()
    return classname


def get_class_methods(classname):
    """Get all methods for a given class

    Parameters
    ----------
    classname : str
        Name of class

    Returns
    -------
    list[str]
    """
    # manually handle otherwise entire infinite recursion
    if "LorentzVector" in classname:
        return LORENTZVECTOR_METHODS[:]
    if classname.startswith("ROOT"):
        return []

    # resolve handle nested classes e.g. Muon.SimType
    parts = classname.strip().split(".")
    this_class = ROOT
    for p in parts:
        this_class = getattr(this_class, p)
    # using ismethod avoids enum values
    methods = [name for name, type_obj
               in inspect.getmembers(this_class, predicate=inspect.ismethod)
               if not name.startswith("_")]
    return methods


def get_class_properties(classname):
    """Get all properties for a given class

    A property can be an enum value, or a class property
    e.g. source_candidate.E

    Parameters
    ----------
    classname : str
        Name of class

    Returns
    -------
    list[str]
    """
    # TODO: unify with get_class_methods as too much overlap?
    if "lorentzvector" in classname.lower():
        return []
    if classname.startswith("ROOT"):
        return []

    # resolve nested classes e.g. Muon.SimType
    parts = classname.strip().split(".")
    this_class = ROOT
    for p in parts:
        this_class = getattr(this_class, p)
    properties = []
    for k, v in vars(this_class).items():  # can't use getmembers for e.g. source_candidate
        if k.startswith("_"):
            continue
        if not isinstance(v, ROOT.PropertyProxy):
            continue
        properties.append(k)
    return properties


def add_list_of_hists(branch_name, branch_type, class_infos, hist_list, do_properties=True):
    """Add list of hists corresponding to class getters, does it recursively

    Parameters
    ----------
    branch_name : str
        Name of branch
    branch_type : str
        Branch class
    class_infos : dict
        Dict of getters/properties for each class
    hist_list : list[str]
        List of nested getters corresponding to a histogram that will be updated
    do_properties : bool, optional
        If True, also add in class properties (filtered)
    """
    join_char = "."
    if branch_type in BUILTIN_TYPES:
        # trivial type, need to keep this for recursive scenario?
        hist_list.append(branch_name)
    else:
        # add getter methods
        for method_name, return_type in class_infos[branch_type]['methods'].items():
            return_type = unvectorise_classname(return_type)
            if return_type in BUILTIN_TYPES or return_type not in class_infos:
                # trivial type, can therefore add and stop there
                hist_list.append(join_char.join([branch_name, method_name]))
            else:
                # return type is a class, so must go through all of its methods as well
                add_list_of_hists(join_char.join([branch_name, method_name]), return_type, class_infos, hist_list, do_properties)

        # add properties e.g. enums
        # only do this for properties of select classes eg source_candidate,
        # not enums, which are more complicated and get taken care of separately
        if do_properties and branch_type in ['source_candidate']:
            for prop in class_infos[branch_type]['properties']:
                hist_list.append(join_char.join([branch_name, prop]))


def get_xy(graph):
    """Return lists of x, y points from a graph"""
    xarr = list(array('d', graph.GetX()))
    yarr = list(array('d', graph.GetY()))
    return xarr, yarr


def plot_hists(h1, stats1, h2, stats2, output_dir=".", canvas_size=(800, 600), fmt='pdf', prepend="", append=""):
    """Make a (comparison) plot from histogram h1 + stats box stats1, and h2+stats2 and save to file.

    h1/stats1 or h2/stats2 can be None, in which case a simpler plot is made
    from non-Null objects.

    Plot saved as <output_dir>/<prepend><method_str><append>.<fmt>
    where method_str is extracted from the histogram name.

    Parameters
    ----------
    h1 : ROOT.TH1, optional
        If None, skip hist
    stats1 : ROOT.TPaveStats, optional
        If None, skip stats box
    h2 : ROOT.TH1, optional
        If None, skip hist
    stats2 : ROOT.TPaveStats, optional
        If None, skip stats box
    output_dir : str
        Output directory for plot
    fmt : str, optional
        Description
    prepend : str, optional
        Description
    append : str, optional
        Description
    """
    if not h1 and not h2:
        print("No TH1 object - not plotting")
        return

    hname = h1.GetName().replace("h1_", "") if h1 else h2.GetName().replace("h2_", "")
    c = ROOT.TCanvas("c"+hname, "", *canvas_size)
    c.SetTicks(1, 1)

    # Check if our version of ROOT has TRatioPlot
    do_ratioplot = True
    try:
        ROOT.TRatioPlot
    except AttributeError:
        do_ratioplot = False

    # Style hists
    h1_colour, h2_colour = HIST_COLOURS[0:2]
    if h1:
        h1.SetLineColor(h1_colour)
        h1.SetLineWidth(2)
    if h2:
        h2.SetLineColor(h2_colour)
        h2.SetLineStyle(2)
        if h1:
            h2.SetLineWidth(0)
        h2.SetMarkerColor(h2_colour)
        h2.SetMarkerStyle(33)
        h2.SetMarkerSize(1.5)

    # Do final plotting
    if do_ratioplot and h1 and h2:
        # Clone h1, since a bug in TRatioPlot will screw up h1
        # and change e.g. GetMean()
        rp = ROOT.TRatioPlot(h1.Clone(ROOT.TUUID().AsString()), h2.Clone(ROOT.TUUID().AsString()))
        rp.SetGridlines(array('d', [1.]), 1)
        # Set margins so that we can fit the stats box off the plot
        rp.SetRightMargin(0.18)
        rp.SetUpTopMargin(0.1)
        rp.SetGraphDrawOpt("ALP")
        rp.Draw("grid")

        if h1.GetEntries() > 0 and h2.GetEntries() > 0:
            lower_gr = rp.GetLowerRefGraph()
            # Reset y errors to 0 in ratio graph as not useful
            for i in range(0, lower_gr.GetN()):
                exl = lower_gr.GetErrorXlow(i)
                exh = lower_gr.GetErrorXhigh(i)
                lower_gr.SetPointError(i, exl, exh, 0, 0)
            lower_gr.SetLineColor(ROOT.kRed)
            ratio_x, ratio_y = get_xy(lower_gr)
            # GetM[ax|in]imum() doesn't work here
            default_min, default_max = 0.8, 1.2
            if len(ratio_y) > 0:
                lower_gr.SetMinimum(max(default_min, 0.99 * min(ratio_y)))
                lower_gr.SetMaximum(min(default_max, 1.01 * max(ratio_y)))
            else:
                lower_gr.SetMinimum(default_min)
                lower_gr.SetMaximum(default_max)

        rp.GetLowerRefYaxis().SetTitle("h1 / h2")
        rp.GetLowerRefYaxis().SetNdivisions(202, False)
        c.Update()
    else:
        xtitle, ytitle = "", ""
        if h1:
            xtitle = h1.GetXaxis().GetTitle()
            ytitle = h1.GetYaxis().GetTitle()
        elif h2:
            xtitle = h2.GetXaxis().GetTitle()
            ytitle = h2.GetYaxis().GetTitle()
        hst = ROOT.THStack("hst"+hname, ";"+xtitle+";"+ytitle)
        if h1:
            hst.Add(h1)
        if h2:
            hst.Add(h2)
        # Set margins so that we can fit the stats box off the plot
        ROOT.gPad.SetRightMargin(0.18)
        ROOT.gPad.SetTopMargin(0.1)
        hst.Draw("NOSTACK HIST")

    c.cd()

    # Format stats boxes & draw
    fmt_str = "1.2g"
    if stats1:
        stats1.SetY1NDC(0.72)
        stats1.SetX1NDC(0.825)
        stats1.SetX2NDC(0.99)
        stats1.SetY2NDC(0.9)
        stats1.SetStatFormat(fmt_str)
        stats1.SetTextColor(h1_colour)
        stats1.GetListOfLines()[0].SetTitle("h1 (new)")
        stats1.Draw()

    if stats2:
        stats2.SetY1NDC(0.52)
        stats2.SetX1NDC(0.825)
        stats2.SetX2NDC(0.99)
        stats2.SetY2NDC(0.7)
        stats2.SetStatFormat(fmt_str)
        stats2.SetTextColor(h2_colour)
        stats2.GetListOfLines()[0].SetTitle("h2 (ref)")
        stats2.Draw()

    c.Modified()

    output_filename = "%s%s%s.%s" % (prepend, hname, append, fmt)
    output_name = os.path.join(output_dir, output_filename)
    c.SaveAs(output_name)


def do_ttree_draw(tree1, tree2, method_str):
    """Draw histogram(s) of variable method_str from tree1 & tree2
    using TTree:Draw(). Both are optional, if None skips that tree.
    method_str must be fairly simple, and not have more than
    one variable-size vector in it.

    If both tree1 and tree2 are valid, the histograms are made such that
    the x axis range covers the entire range for both trees.

    Parameters
    ----------
    tree1 : ROOT.TTree, optional
        If None, skip tree
    tree2 : ROOT.TTree, optional
        If None, skip tree
    method_str : str
        Method string that starts with collection name
        e.g. slimmedJets.pt()

    Returns
    -------
    ROOT.TH1, ROOT.TPaveStats, ROOT.TH1 (optional), ROOT.TPaveStats (optional)
        The histograms with their TPaveStats
        (which are difficult to get in the first place)
    """

    # Don't do the SetBranchStatus trick here as it doesn't work

    # print("TTree drawing", method_str)
    hname_clean = method_str.replace("()", "")
    c = ROOT.TCanvas("c"+hname_clean, "", 800, 600)

    xmin, xmax = 999999999, -999999999

    h1name = "h1_%s" % hname_clean
    if tree1:
        h1name_tmp = h1name + "_tmp"
        tree1.Draw(method_str + ">>" + h1name_tmp, "", "goff")
        # do it this way to let ROOT decide upon the axis range
        h1_tmp = ROOT.TH1F(ROOT.gROOT.FindObject(h1name_tmp))

        # Figure out range from ROOT hist info
        ax1 = h1_tmp.GetXaxis()
        xmin, xmax = ax1.GetXmin(), ax1.GetXmax()

    h2name = "h2_%s" % hname_clean
    if tree2:
        h2name_tmp = h2name + "_tmp"
        tree2.Draw(method_str + ">>" + h2name_tmp, "", "goff")
        h2_tmp = ROOT.TH1F(ROOT.gROOT.FindObject(h2name_tmp))
        ax2 = h2_tmp.GetXaxis()
        xmin2, xmax2 = ax2.GetXmin(), ax2.GetXmax()

        # update min/max x range
        xmin = min(xmin, xmin2)
        xmax = max(xmax, xmax2)

    delta = xmax - xmin
    xmin -= 0.1 * delta
    xmax += 0.1 * delta

    # Now remake histograms, this time according to our binning
    # Yes this is kinda repetitive, but a priori we didn't know the right range
    h1, stats1 = None, None
    h2, stats2 = None, None
    nbins = 50
    if tree1:
        h1 = ROOT.TH1F(h1name, ";%s;N" % method_str, nbins, xmin, xmax)
        tree1.Draw(method_str + ">>" + h1name)
        h1.Draw("HIST")
        c.Update()
        # Get stat boxes for repositioning
        # Draw hist by itself to get it, then plot them together afterwards
        stats1 = h1.GetListOfFunctions().FindObject("stats").Clone("stats1")
        h1.SetStats(0)

    if tree2:
        h2 = ROOT.TH1F(h2name, ";%s;N" % method_str, nbins, xmin, xmax)
        tree2.Draw(method_str + ">>" + h2name)
        h2.Draw("HIST")
        c.Update()
        stats2 = h2.GetListOfFunctions().FindObject("stats").Clone("stats2")
        h2.SetStats(0)

    return h1, stats1, h2, stats2


def get_compounded_return_types(method, tree_info, class_infos):
    """For a given chained set of methods on a collection, return
    a list of the return types, each entry corresponding to one of the methods.

    If collection does not exist in tree_info, return empty list

    Assumes float type for a property (probably shouldn't!)

    e.g. slimmedJets_SoftDrop.subjets().pt() -> [TopJet, Jet, float]

    Parameters
    ----------
    method : str
        Total chained method e.g. slimmedJets.subjets().pt()
    tree_info : list[BranchInfo]
        List with info about branch collections in tree
    class_infos : dict
        Dict with info about methods for classes

    Returns
    -------
    list[str]
        List of return types, empty if collection does not exist in tree_info
    """
    # Have to go through each level of access, and check return type
    method_parts = method.split(".")
    # First figure out what type this branch is and if it even exists in the tree
    br_info = [x for x in tree_info if x.name == method_parts[0]]
    if len(br_info) > 0:
        br_info = br_info[0]
    else:
        return []

    return_types = [br_info.classname]
    this_type = br_info.classname
    for hp in method_parts[1:]:
        if hp.endswith("()"):  # distinguish method from property
            this_type = class_infos[unvectorise_classname(this_type)]['methods'][hp]
            return_types.append(this_type)
        else:
            # silly default, but no idea how to get actual type of properties
            # hopefully is never a vector
            return_types.append("float")
    return return_types


def safe_iter(thing):
    """Return an iterable over `thing`, regardless of whether
    it is iterable or a single value
    """
    try:
        return iter(thing)
    except TypeError:
        return iter([thing])


def safe_next(thing):
    """Call next() on `thing`: if iterable, then returns next in sequence as usual;
    if single object returns object.

    Also returns bool as to whether it was a single objext or iterable
    """
    try:
        return next(thing), True
    except TypeError:
        return thing, False


def get_data(tree, method_str):
    """Get data from chained method in `method_str` by iterating over the tree.
    This is designed for method chains that include methods that return vectors,
    since TTree.Draw can't handle them. However it is naturally slower.

    Parameters
    ----------
    tree : ROOT.TTree
        tree to iterate ove
    method_str : str
        Chained method string that starts with collection name
        e.g. slimmedJets.btaginfo().TrackEta()

    Yields
    ------
    [float]
        Data in the tree
    """
    hparts = method_str.split(".")
    if len(hparts) < 2:
        raise RuntimeError("Improper tree variable, should be xxx.yyy at least")

    # To handle any number of chained method calls, we have to build a stack
    # of methods, and of objects. The latter is updated each time the lower
    # iteration finishes.
    # Inspired by https://stackoverflow.com/a/45503317
    methods = []
    for p in hparts[1:]:
        if p.endswith("()"):
            methods.append(methodcaller(p.replace("()", "")))
        else:
            methods.append(attrgetter(p))

    collection_name = hparts[0]
    # Using br.GetEntry() and not tree.GetEntry() offers a BIG speedup,
    # since it will otherwise cache *every* collection
    #
    # WARNING: you cannot use:
    # tree.SetBranchStatus("*", 0)
    # tree.SetBranchStatus(collection_name, 1)
    # since the latter doesn't correctly reinstate the branch, for unknown reason
    # So this is the only way to activate a specific branch
    br = tree.GetBranch(collection_name)

    for ind in range(tree.GetEntries()):
        br.GetEntry(ind)
        # Using safe_iter here is required since our collection might be
        # a single object (genInfo) or a vector (slimmedJets)
        for obj in safe_iter(getattr(tree, collection_name)):
            xs = [safe_iter(methods[0](obj))]
            while xs:
                try:
                    x, iterable = safe_next(xs[-1])
                    if len(xs) < (len(methods)):
                        # Build up the stack of objects: we call the relevant
                        # method on the previous layer's object (x)
                        xs.append(safe_iter(methods[len(xs)](x)))
                    else:
                        # Here we have all the necessary layers,
                        # so we just return the value
                        yield x
                    # handle case of singel value, needs StopIteration to exit
                    if not iterable:
                        raise StopIteration
                except StopIteration:
                    xs.pop()


def do_event_loop_hists(tree1, tree2, method_str):
    """Make histograms & stats boxes from tree1 & tree2 using data
    return by the method_str called on each tree. Each tree can be None, in which
    case None is returns for its corresponding hist & stats box.

    Parameters
    ----------
    tree1 : ROOT.TTree, optional
    tree2 : ROOT.TTree, optional
    method_str : str
        Chained method string that starts with collection name
        e.g. slimmedJets.btaginfo().TrackEta()

    Returns
    -------
    ROOT.TH1, ROOT.TPaveStats, ROOT.TH1, ROOT.TPaveStats
    """
    hname_clean = method_str.replace("()", "")
    c = ROOT.TCanvas("ctmp"+hname_clean, "", 800, 600)

    nbins = 50
    data1 = []
    if tree1:
        data1 = [d for d in get_data(tree1, method_str)]

    data2 = []
    if tree2:
        data2 = [d for d in get_data(tree2, method_str)]

    # Figure out axis range using both hists is possible
    xmin, xmax = 0, 1
    if len(data1) > 0 or len(data2) > 0:
        xmin = min(chain(data1, data2))
        xmax = max(chain(data1, data2))
        delta = xmax - xmin
        # bit of extra padding
        xmin -= (0.1 * delta)
        xmax += (0.1 * delta)

    # Make hists
    h1, stats1 = None, None
    if tree1:
        h1name = "h1_%s" % (hname_clean)
        h1 = ROOT.TH1F(h1name, ";%s;N" % method_str, nbins, xmin, xmax)
        for d in data1:
            h1.Fill(d)
        h1.Draw("HIST")
        c.Update()
        # Get stat boxes for repositioning
        # Draw hist by itself to get it, then plot them together afterwards
        stats1 = h1.GetListOfFunctions().FindObject("stats").Clone("stats1")
        h1.SetStats(0)

    h2, stats2 = None, None
    if tree2:
        h2name = "h2_%s" % (hname_clean)
        h2 = ROOT.TH1F(h2name, ";%s;N" % method_str, nbins, xmin, xmax)
        for d in data2:
            h2.Fill(d)
        h2.Draw("HIST")
        c.Update()
        stats2 = h2.GetListOfFunctions().FindObject("stats").Clone("stats2")
        h2.SetStats(0)

    c.Clear()

    return h1, stats1, h2, stats2


def make_hists(tree1, tree1_info, class_infos1, tree2, tree2_info, class_infos2, method_str):
    """Make histograms & stats boxes from tree1 & tree2
    from chained methods on a collection name passed as `method_str`.

    One or both trees can be None, in which case None is returned
    for their respective hist & stats box.

    If the collection does not exist in a tree, it will return None
    for its respective hist & stats box.

    Uses the faster TTree:Draw() mechanism if possible, otherwise loop over
    all events in the tree for more complex method calls.

    Parameters
    ----------
    tree1 : ROOT.TTree
        If None, ignore this tree
    tree1_info : list[BranchInfo]
        List with info about branch collections in tree1
    class_infos1 : dict
        Dict with info about methods for classes in tree1
    tree2 : ROOT.TTree
        If None, ignore this tree
    tree2_info : list[BranchInfo]
        List with info about branch collections in tree2
    class_infos2 : dict
        Dict with info about methods for classes in tree2
    method_str : str
        Chained method string that begins with the collection name,
        e.g. slimmedJets.btaginfo().TrackEta()

    Returns
    -------
    ROOT.TH1, ROOT.TPaveStats, ROOT.TH1, ROOT.TPaveStats
    """
    # To be efficient, use TTree.Draw() where possible.
    # Only resort back to loops if >= 1 methods return a variable-size collection,
    # as ROOT can't handle this
    # i.e. figure out how many of these return types are vectors
    # We don't care about the collection type e.g. genInfo.binningValues()
    # can't use TTree.Draw()
    n_vector_methods1 = 0
    return_types1 = []
    if tree1 and tree1_info and class_infos1:
        return_types1 = get_compounded_return_types(method_str, tree1_info, class_infos1)
        n_vector_methods1 = sum(["vector<" in rt for rt in return_types1[1:]])

    # do it separately for tree2 as class structure/types may have changed
    n_vector_methods2 = 0
    return_types2 = []
    if tree2 and tree2_info and class_infos2:
        return_types2 = get_compounded_return_types(method_str, tree2_info, class_infos2)
        n_vector_methods2 = sum(["vector<" in rt for rt in return_types2[1:]])

    h1, stats1, h2, stats2 = None, None, None, None
    MAX_VECTOR_METHODS = 0
    # Pass None if collection doesn't exist in tree, or tree doesn't exist
    tree1 = tree1 if (tree1 and len(return_types1) > 0) else None
    tree2 = tree2 if (tree2 and len(return_types2) > 0) else None
    if not tree1 and not tree2:
        return h1, stats1, h2, stats2

    if n_vector_methods1 <= MAX_VECTOR_METHODS and n_vector_methods2 <= MAX_VECTOR_METHODS:
        # Quick ttree draw - need both in one method to make x axes same
        h1, stats1, h2, stats2 = do_ttree_draw(tree1, tree2, method_str)
    else:
        # Iterate over events and objects in collections
        h1, stats1, h2, stats2 = do_event_loop_hists(tree1, tree2, method_str)
    return h1, stats1, h2, stats2


class HistSummary(object):

    def __init__(self, name, description):
        """Simple class to hold info about summary between 2 hists

        Parameters
        ----------
        name : str
            Classification of summary
        description : str
            Detailed description
        """
        self.name = name.upper().replace(" ", "_")
        self.description = description

    def __eq__(self, other):
        return self.name == other.name

    def __repr__(self):
        return "HistSummary(%s, %s)" % (self.name, self.description)

    def __str__(self):
        return "HistSummary(%s, %s)" % (self.name, self.description)

    def __lt__(self, other):
        return self.name < other.name


def analyse_hists(hist1, hist2):
    """Analyse the 2 histograms and return status string

    Parameters
    ----------
    hist1 : ROOT.TH1, optional
    hist2 : ROOT.TH1, optional

    Returns
    -------
    HistSummary
        Summary info in the form of a HistSummary object
    """

    if hist2 is None and hist1 is None:
        return HistSummary("BOTH_EMPTY", "Both hists not available")

    if hist2 is None or hist1 is None:
        return HistSummary("ONE_EMPTY", "One hist not available")

    n_entries1 = hist1.GetEntries()
    n_entries2 = hist2.GetEntries()

    if n_entries1 == 0 or n_entries2 == 0:
        return HistSummary("NO_ENTRIES", "One or both hists has 0 entries")

    if n_entries2 != n_entries1:
        return HistSummary("DIFF_ENTRIES", "Differing number of entries")

    # Check x axis range to see if unusually large, or occupies only large numbers
    # X range should be same for both by design
    xmin, xmax = hist1.GetXaxis().GetXmin(), hist1.GetXaxis().GetXmax()
    delta = xmax - xmin
    # hopefully nothing spans that range
    range_lim = 1.0E10
    if delta > range_lim:
        return HistSummary("VERY_LARGE_RANGE", "Values have very large range (> %g)" % range_lim)

    # Maybe range is small, but all values are very very large (or v.v.small)
    if xmax > range_lim or xmin < -range_lim:
        return HistSummary("EXTREME_VALUES", "x axis has very large values (+- %g)" % range_lim)

    # FIXME better float comparison to avoid floating point errors
    mean1, mean2 = hist1.GetMean(), hist2.GetMean()
    if mean1 != mean2:
        return HistSummary("DIFF_MEANS", "Differing means")

    rms1, rms2 = hist1.GetRMS(), hist2.GetRMS()
    if (mean1 == 0 and rms1 == 0) or (mean2 == 0 and rms2 == 0):
        return HistSummary("ZERO_VALUE", "One or both hists have only 0s")

    if rms1 != rms2:
        return HistSummary("DIFF_RMS", "Differing RMSs")

    if rms1 == 0 or rms2 == 0:
        return HistSummary("ZERO_RMS", "One or both RMSs are 0: stores same value")

    return HistSummary("SAME", "Histograms are the same (lowest priority)")


def check_tobj(tobj):
    """Check if TObject is valid, if not raise IOError"""
    if tobj == None or tobj.IsZombie():
        raise IOError("Cannot access %s" % tobj.GetName())


def parse_tree(tree):
    """Parse TTree by storing collections, types & their methods,
    and compiling list of histograms corresponding to object getters.

    Parameters
    ----------
    tree : ROOT.TTree

    Returns
    -------
    list[BranchInfo], dict, list[str]
        List of collections in tree,
        info about classes & methods/properties in tree,
        list of histograms
    """
    # Get top level branches
    tree_info = []
    store_branches(tree, tree_info, do_recursive=False, indent=None)

    # Since ROOT can't see beyond 2 levels of classes,
    # let's manually figure out class methods
    class_infos = OrderedDict()
    hist_list = []
    for binfo in tree_info:
        classname = unvectorise_classname(binfo.classname)
        if classname not in class_infos and classname not in BUILTIN_TYPES:
            get_class_getters_info(classname, class_infos, do_recursive=True)

        # add histogram names to be processed
        add_list_of_hists(binfo.name, classname, class_infos, hist_list, do_properties=True)

    return tree_info, class_infos, hist_list


def print_tree_summary(tree_info, class_info, label):
    print("-" * 80)
    print("%s collections:" % (label))
    print("-" * 80)
    [print(t) for t in tree_info]
    print("-" * 80)
    print("%s class info:" % (label))
    print("-" * 80)
    print(json.dumps(class_info, indent=2))
    print("-" * 80)


def save_to_json(json_data, hist_status, output_filename):
    """Save plot info to JSON

    Parameters
    ----------
    json_data : dict
        Main dict about added hists etc
    hist_status : dict
        Dict of {histname: HistSummary}
    output_filename : str
    """
    del json_data['common_hists']
    # Store histograms grouped by status
    statuses = sorted(list(set(hist_status.values())))
    status_dict = {}
    for status in statuses:
        status_dict[status.name] = {
                                    "description": status.description,
                                    "number": 0,
                                    "names": []
                                   }

    added_removed_hists = json_data['added_hists'] + json_data['removed_hists']
    for hist_name, status in hist_status.items():
        if hist_name in added_removed_hists:
            continue
        status_dict[status.name]['number'] += 1
        status_dict[status.name]['names'].append(hist_name)

    json_data['comparison'] = status_dict

    def _convert_entry(my_dict, key):
        """Replace basic list with dict of list and length"""
        col = my_dict[key]
        new_entry = {"number": len(col), "names": col}
        my_dict[key] = new_entry

    # Reorganise the added/removed hists to also store #s, easier for table
    _convert_entry(json_data, 'added_collections')
    _convert_entry(json_data, 'removed_collections')
    _convert_entry(json_data, 'added_hists')
    _convert_entry(json_data, 'removed_hists')

    with open(output_filename, 'w') as jf:
        json.dump(json_data, jf, indent=2, sort_keys=True)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("filename",
                        help='ROOT Ntuple filename (labelled "new")')
    parser.add_argument("--compareTo", dest='compare',
                        help='Optional reference comparison ROOT filename (labelled "ref")')
    default_tree = "AnalysisTree"
    parser.add_argument("--treeName",
                        help="Name of TTree, defaults to %s" % default_tree,
                        default=default_tree)
    default_output_dir = "treePlots"
    parser.add_argument("--outputDir",
                        help="Output directory for plots, defaults to %s" % default_output_dir,
                        default=default_output_dir)
    default_json_filename = "this.json"
    parser.add_argument("--json",
                        help="Output JSON filename template used for class & plot JSONs, defaults to %s" % default_json_filename,
                        default=default_json_filename)
    default_fmt = "pdf"
    parser.add_argument("--fmt",
                        help="Output plot file format, defaults to %s. "
                             "Pass space-separated list to save file in each format" % default_fmt,
                        nargs="+",
                        default=default_fmt)
    parser.add_argument("--thumbnails",
                        help="Make thumbnail plots in <outputDir>/thumbnails",
                        action='store_true')
    parser.add_argument("--verbose", "-v",
                        help="Printout extra info",
                        action='store_true')
    args = parser.parse_args()

    if not os.path.isfile(args.filename):
        raise IOError("Cannot find filename %s" % args.filename)

    f1 = ROOT.TFile(args.filename)
    tree1 = f1.Get(args.treeName)
    check_tobj(tree1)

    tree1_info, class_infos1, hist_list1 = parse_tree(tree1)

    if args.verbose:
        print_tree_summary(tree1_info, class_infos1, 'tree1')

    print(len(hist_list1), "hists in main tree")

    json_data = {
        "added_collections": [],
        "removed_collections": [],
        "added_hists": [],
        "removed_hists": []
    }

    # Do the same thing for other file if it exists
    tree2_info = []
    class_infos2 = OrderedDict()
    hist_list2 = []
    tree2 = None
    if args.compare:
        if not os.path.isfile(args.compare):
            raise IOError("Cannot find filename %s" % args.compare)
        f2 = ROOT.TFile(args.compare)
        tree2 = f2.Get(args.treeName)
        check_tobj(tree2)

        tree2_info, class_infos2, hist_list2 = parse_tree(tree2)

        if args.verbose:
            print_tree_summary(tree2_info, class_infos2, 'tree2')

        print(len(hist_list2), "hists in compareTo tree")

        # Store added/removed collections
        # Added/removed are defined realtive to the tree passed as --compareTo
        cols1 = set([b.name for b in tree1_info])
        cols2 = set([b.name for b in tree2_info])
        added = sorted(list(cols1 - cols2))
        removed = sorted(list(cols2 - cols1))
        json_data['added_collections'].extend(added)
        json_data['removed_collections'].extend(removed)

        # Store added/removed hists
        hists1 = set(hist_list1)
        hists2 = set(hist_list2)
        added_hists = sorted(list(hists1 - hists2))
        removed_hists = sorted(list(hists2 - hists1))
        json_data['added_hists'].extend(added_hists)
        json_data['removed_hists'].extend(removed_hists)
        json_data['common_hists'] = sorted(list(hists1 & hists2))

    # Setup output dirs
    if not os.path.isdir(args.outputDir):
        os.makedirs(args.outputDir)

    thumbnails_dir = os.path.join(args.outputDir, "thumbnails")
    if args.thumbnails and not os.path.isdir(thumbnails_dir):
        os.makedirs(thumbnails_dir)

    hist_status = OrderedDict()

    # Make & plot (comparison) hists
    common_hists = sorted(json_data.get('common_hists', hist_list1))  # If only ref file, just do all
    added_hists = json_data.get('added_hists', [])
    removed_hists = json_data.get('removed_hists', [])
    all_hists = list(chain(common_hists, added_hists, removed_hists))

    print("Producing", len(all_hists), "hists")
    json_data['total_number'] = len(all_hists)

    # Use tqdm to get nice prgress bar, and add hist name if verbose,
    # padded to keep constant position for progress bar
    # disable on non-TTY
    pbar = tqdm(all_hists, disable=None)
    max_len = max(len(l) for l in all_hists)
    fmt_str = "{0: <%d}" % (max_len+2)
    for method_str in pbar:
        if args.verbose:
            pbar.set_description(fmt_str.format(method_str))

        # Make histograms
        hist1, stats1, hist2, stats2 = make_hists(tree1, tree1_info, class_infos1,
                                                  tree2, tree2_info, class_infos2,
                                                  method_str)

        # Plot to file
        if hist1 or hist2:
            for fmt in args.fmt:
                plot_hists(hist1, stats1, hist2, stats2,
                           args.outputDir,
                           canvas_size=(800, 600),
                           fmt=fmt,
                           prepend="", append="")
            if args.thumbnails:
                plot_hists(hist1, stats1, hist2, stats2,
                           thumbnails_dir,
                           canvas_size=(300, 200),
                           fmt='gif',
                           prepend="", append="")

        # Do comparison
        status = analyse_hists(hist1, hist2)
        hist_status[method_str] = status

    print(len(hist_status), "plots produced")

    # Save JSON data
    json_dir = os.path.dirname(args.json)
    json_file = os.path.basename(args.json)
    if class_infos1:
        with open(os.path.join(json_dir, "classes1_"+json_file), 'w') as jf:
            json.dump(class_infos1, jf, indent=2, sort_keys=True)

    if class_infos2:
        with open(os.path.join(json_dir, "classes2_"+json_file), 'w') as jf:
            json.dump(class_infos2, jf, indent=2, sort_keys=True)

    if args.compare:
        save_to_json(json_data, hist_status, output_filename=os.path.join(json_dir, "plots_"+json_file))
