# Streptavidin autoindexing tests --- instructions

1. Modify $PYTHONPATH in `strep.py` to point to the swig directory in the NSXTool build directory:
   ```sys.path.append("/home/zamaan/codes/nsxtool/nsxtool/build/swig")```
2. Run the script on the included data files:
   ```strep.py -f p15843_00027666.tiff p15843_00027667.tiff p15843_00027668.tiff ```
