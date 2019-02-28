#!/usr/bin/env python

"""
This script goes through various branches of UHH2 code and looks for NTuple
files in use taken from common/datasets/*.xml

This is to ensure we keep don't use lots of space unnecessarily.

The code checks out a fresh copy of UHH2, then checks out each branch, looks
for all ROOT filenames in the XML files, and saves a list of them to a txt file.

The user should update THESE_BRANCHES as appropriate
"""


from __future__ import print_function
import os
import sys
import subprocess
import uuid
import shutil


# Only check these branch names
THESE_BRANCHES = [
    "RunII_102X_v1",
    "RunII_101_v1",
    "RunII_94X_v2",
    "RunII_94X_v1",
    "RunII_80X_v5",
    "RunII_80X_v4",
    "RunII_80X_v3",
]


# Set this to the remote name that will be used for the central UHH2 repo
REMOTE_NAME = "UHH"


def init_repo(repo_url, clone_dir):
    if os.path.isdir(clone_dir):
        print(clone_dir+" already exists, deleting")
        shutil.rmtree(clone_dir)
    os.makedirs(clone_dir)
    os.chdir(clone_dir)
    subprocess.check_call("git init", shell=True)
    subprocess.check_call("git remote add "+REMOTE_NAME+" "+repo_url, shell=True)
    subprocess.check_call("git fetch "+REMOTE_NAME, shell=True)


def get_all_remote_branches():
    cmd = "git --no-pager branch -r"
    out = subprocess.check_output(cmd, shell=True)
    return [x.strip() for x in out.decode().splitlines()]


def get_all_local_branches():
    cmd = "git --no-pager branch"
    out = subprocess.check_output(cmd, shell=True)
    return [x.strip().strip("*").strip() for x in out.decode().splitlines()]


def checkout_branch(remote_branch_name, local_branch_name):
    cmd = "git fetch -u %s %s:%s" % (REMOTE_NAME, remote_branch_name, local_branch_name)
    subprocess.check_call(cmd, shell=True)
    print("Checking out", remote_branch_name, "to", local_branch_name)
    cmd = "git checkout %s" % (local_branch_name)
    subprocess.check_call(cmd, shell=True)


def find_xml_files():
    xml_filenames = []
    for root, dirs, files in os.walk('common/datasets'):
        for filename in files:
            if os.path.splitext(filename)[1] == ".xml":
                xml_filenames.append(os.path.join(root, filename))
    return xml_filenames


def get_root_files_from_xml(xml_filename):
    root_filenames = []
    with open(xml_filename) as f:
        for line in f:
            line = line.strip()
            if line.startswith(("<!--", "-->")):
                continue
            this_line = line.replace('<In FileName="', '').replace('" Lumi="0.0"/>', '')
            if this_line.startswith(("/nfs", "/pnfs")):
                root_filenames.append(this_line)
    return root_filenames


def save_list_to_file(this_list, output_filename):
    with open(output_filename, "w") as f:
        f.write("\n".join(this_list))


def main():

    # Setup UHH2 in clean directory avoid any contamination
    deploy_dirname = "UHHCounting"
    init_repo("https://github.com/UHH2/UHH2.git", deploy_dirname)

    # Figure out which branches to look at based on what user wants,
    # and what is available
    our_list_of_branches = [REMOTE_NAME+"/"+x for x in THESE_BRANCHES]

    list_of_remote_branches = get_all_remote_branches()
    list_of_local_branches = get_all_local_branches()

    important_branches = sorted(list(set(our_list_of_branches) & set(list_of_remote_branches)))
    print("Only looking in branches:", important_branches)

    for remote_branch in important_branches[:]:
        all_root_files = []
        remote_branch = remote_branch.lstrip(REMOTE_NAME+"/")
        local_branch_name = remote_branch
        checkout_branch(remote_branch, local_branch_name)
        xml_files = find_xml_files()
        these_root_files_lists = [get_root_files_from_xml(x) for x in xml_files]
        for l in these_root_files_lists:
            all_root_files.extend(l)

        all_root_files = sorted(list(set(all_root_files)))
        file_log_filename = "ntuple_filenames_"+remote_branch+".txt"
        save_list_to_file(all_root_files, "../"+file_log_filename)  # use .. as we're in the UHH repo
        print("Found", len(all_root_files), "ntuples, list saved to", file_log_filename)

        all_root_files_dirs = sorted(list(set([os.path.dirname(f) for f in all_root_files])))
        dir_log_filename = "ntuple_dirnames_"+remote_branch+".txt"
        save_list_to_file(all_root_files_dirs, "../"+dir_log_filename)  # use .. as we're in the UHH repo
        print("Found", len(all_root_files_dirs), "ntuple dirs, list saved to", dir_log_filename)

    return 0


if __name__ == "__main__":
    sys.exit(main())
