#!/bin/sh
find . \( -name \*.h -or -name \*.cpp -or -name \*.h.in -or -name \*.cpp.in \) -and \( -path ./qcr/\* -or -path ./core/\* -or -path ./gui/\* -or -path ./main/\* \) | sort
