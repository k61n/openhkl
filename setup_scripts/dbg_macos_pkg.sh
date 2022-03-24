#!/bin/bash
# *** Debug self-contained zip-package for MacOS ***
# TODO: add detailed descript and copyright note

HELP="* Usage: bash dbg_macos_pkg.sh <log-file>"

if [ -z "$1" ]
then
    echo "Error: Provide a path for the log file."
    echo "$HELP"
    exit 1
fi

logfile="$1"

echo "Collecting debug info..."
echo -e "# MacOS package log:\n" > $logfile
echo "## Machine:" >> $logfile
uname -a >> $logfile
system_profiler SPSoftwareDataType >> $logfile
echo "## MacPorts" >> $logfile
port installed | grep -iF qt >> $logfile
echo "---" >> $logfile
port -q contents qt5-qtbase | grep -e 'QtCore$' >> $logfile
echo "## Homebrew:" >> $logfile
brew list | grep -iF qt >> $logfile
echo "---" >> $logfile
brew config >> $logfile
echo "## NSXTool dependencies:" >> $logfile
otool -L NSXTool >> $logfile
echo "## NSXTool RPATHs:" >> $logfile
otool -l NSXTool | grep -A2 RPATH >> $logfile
echo "----------" >> $logfile
echo "The debug info written to '$logfile'."

exec=NSXTool
exelog=testexe.log
echo "Testing the executable (log in '$exelog')..."
DYLD_PRINT_LIBRARIES=1 DYLD_PRINT_RPATHS=1 DYLD_PRINT_TO_FILE=$exelog ./$exec >> $logfile 2>&1
echo "Done."
