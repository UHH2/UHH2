#!/usr/bin/env python


"""Loop over XML files, get filenames, look for their info, save to CSV.

Also saves list of missing ntuples.
"""


from __future__ import print_function

import os
import sys
import argparse
import pandas as pd
import numpy as np
from time import sleep


def get_ntuple_filenames_from_xml(full_filename):
    """Yield ntuple filenames from XML file

    Parameters
    ----------
    full_filename : str
        XML file to get ntuples from

    Yields
    ------
    generator
        To iterate over filenames
    """
    with open(full_filename) as f:
        is_comment = False
        for line in f:
            if line.startswith("<!--"):
                is_comment = True
            if line.endswith("-->"):
                is_comment = False
            if is_comment:
                continue
            if line.startswith("<In FileName="):
                this_line = line.strip()
                this_line = this_line.replace('<In FileName="', '')
                this_line = this_line.replace('" Lumi="0.0"/>', '')
                yield this_line


def get_ntuples_from_xml_files(top_directory):
    """Get iterator over ntuples in XML files in a directory.
    Looks recursively through directories for XML files.

    Parameters
    ----------
    top_directory : str

    Yields
    ------
    (str, iterator)
        Returns (relative path of XML file, ntuple filename iterator)
    """
    for (dirpath, dirnames, filenames) in os.walk(top_directory):
        print("Looking in", dirpath)
        for filename in filenames:
            full_filename = os.path.join(dirpath, filename)
            rel_path = os.path.relpath(full_filename, top_directory)
            ntuple_iter = get_ntuple_filenames_from_xml(full_filename)
            yield rel_path, ntuple_iter


def get_user_from_filename(ntuple_filename):
    """Get username from full filepath

    Assumes it comes after .../user/
    e.g. :
    get_user_from_filename("/nfs/dust/cms/user/robin/UHH2/Ntuple_2016v2.root")
    >> robin

    Parameters
    ----------
    ntuple_filename : str

    Returns
    -------
    str
        Username, or None if not found
    """
    if "/user/" not in ntuple_filename:
        return None
    parts = ntuple_filename.split("/")
    ind = parts.index("user")
    if ind == len(parts)-1:
        return None
    return parts[ind+1]


def get_year_from_dir(dirname):
    """Get dataset year from XML filepath.

    Assumes it comes after RunII_*, or is the first part
    e.g.
    get_year_from_dir("../common/dataset/RunII_102X_v1/2017v2/MC_TTbar.xml")
    >> "2017v2"

    Parameters
    ----------
    dirname : str

    Returns
    -------
    str
        Year, or None if not found
    """
    parts = dirname.split("/")
    branch = "RunII_"
    for p in parts:
        if branch in p:
            ind = parts.index(p)
            if ind == len(parts)-1:
                return None
            return parts[ind+1]
    else:
        return parts[0]


def get_all_data(top_dir, missing_filename):
    """Get all Ntuple data

    Parameters
    ----------
    top_dir : str
        Parent directory to look for XML files
    missing_filename : str
        Name for output missing ntuple file

    Returns
    -------
    list[dict]
        List of ntuple info, each entry is a dict
    """
    data = []
    # Save missing file info to separate file
    print("Saving missing file info to", missing_filename)
    with open(missing_filename, "w") as f_missing:
        top_dir = os.path.abspath(top_dir)
        counter = 0
        for ind, (xml_rel_path, ntuple_iter) in enumerate(get_ntuples_from_xml_files(top_dir)):
            first_time = True

            for ntuple_filename in ntuple_iter:

                counter += 1

                if not os.path.isfile(ntuple_filename):
                    if first_time:
                        # If it's the first time we encounter this file,
                        # print it's filename so easier to track down
                        f_missing.write("-"*10 + "\n")
                        f_missing.write("%s\n" % xml_rel_path)
                        f_missing.write("-"*10 + "\n")
                        first_time = False
                    f_missing.write(ntuple_filename)
                    f_missing.write("\n")
                    continue

                # size = np.random.random() * 100  # dummy data for testing
                user = get_user_from_filename(ntuple_filename)
                size = os.path.getsize(ntuple_filename) / (1024.0 * 1024.0)  # to MBytes
                year = get_year_from_dir(xml_rel_path)
                data.append({
                    "xmldir": os.path.dirname(xml_rel_path),
                    "ntuple": ntuple_filename,
                    "size": size,
                    "user": user,
                    "year": year,
                })

                # Sleep every so often to avoid too much stress on filesystem
                if counter % 5000 == 0:
                    print("Done", counter, ", sleeping for 5s...")
                    sleep(5)
    return data


def dataset_info(top_dir, csv_filename):
    """Go through all XML files recursively from top_dir, get file info, save to CSV.

    Parameters
    ----------
    top_dir : str
        Parent directory to look for XML files
    csv_filename : str
        Output CSV filename to use. Also used as template for missing filename.
    """
    # To save missing file info to separate file
    missing_file = os.path.splitext(csv_filename)[0]
    missing_file = missing_file + "_missing.txt"
    data = get_all_data(top_dir=top_dir, missing_filename=missing_file)
    print("Saving to dataframe & CSV...")

    # Convert to pandas dataframe, makes life easier
    df = pd.DataFrame(data)
    df['user'] = df['user'].astype('category')
    df['xmldir'] = df['xmldir'].astype('category')
    df['year'] = df['year'].astype('category')

    # Print out bits of dataframe to check sane
    print(df.head())
    print(df.tail())
    print("Dataframe column dtypes:")
    print(df.dtypes)
    print("Dataframe describe():")
    print(df.describe())
    print("Memory usage:")
    print(df.memory_usage(deep=True))
    print(len(df.index), "entries in dataframe")

    # Save it to CSV
    df.to_csv(csv_filename)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("topDir",
                        help="Top directory to look for XML files. "
                        "All subdirectories will be included, recursively.")
    parser.add_argument("--csv",
                        default="datasetinfo.csv",
                        help="Input/output CSV file.")
    args = parser.parse_args()

    if not os.path.isdir(args.topDir):
        raise IOError("%s does not exist" % args.topDir)

    csv_dir = os.path.dirname(os.path.abspath(args.csv))
    if not os.path.isdir(csv_dir):
        os.path.makedirs(csv_dir)

    dataset_info(top_dir=args.topDir, csv_filename=args.csv)
    sys.exit(0)
