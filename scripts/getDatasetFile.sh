#!/usr/bin/env bash
#
# Get smallest file in dataset that has at least N events
# Usage:
#
#     ./getDatasetFile.sh /a/b/c 1000
#
set -u

dasgoclient -query="file,nevents dataset=$1 | grep file.name, file.nevents" | sort -r -k2 -n  | awk '$2 > '$2 | tail -n 1

