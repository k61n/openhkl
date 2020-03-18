# Streptavidin autoindexing tests --- instructions

1. Modify $PYTHONPATH in `strep.py` to point to the swig directory in the NSXTool build directory:
   ```sys.path.append("/home/zamaan/codes/nsxtool/nsxtool/build/swig")```
2. Run the script on the a set of .tiff raw data files
   ```autoindextest.py --name strep --files *.tiff -n 4```
   This will do all processing steps for all sets of 4 *contiguous* numors up to and including the autoindexing step.
