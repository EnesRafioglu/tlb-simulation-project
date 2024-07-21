#!/bin/bash

set -e

# Build in release mode
make release
./tlb

printf "\n\n***Build successful***\n\n"
