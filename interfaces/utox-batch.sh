#!/bin/bash

#
# Batch mode interface to Utox - Sample code with multiprocessing support
#
# Copyright (c) 2011, Jouke Witteveen <j.witteveen@gmail.com>
# All rights reserved.
#
# Usage: utox-batch.sh image-file [...]
#

# Add -v for extra verbosity (output of all processes gets mixed).
UTOX="$(dirname "$0")/../source/utox"
MAXPROCS=4

for f in "$@"; do printf "%q\0" "$f"; done |\
  xargs -0 -n 1 -P $MAXPROCS -t -I {} $UTOX -f "{}" -o "{}.xmi"
