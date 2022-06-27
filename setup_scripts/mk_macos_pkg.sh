#!/bin/bash
set -o noclobber  # avoid overwriting existing files
# TODO: add detailed descript and copyright note

# *** Make a self-contained zip-package for MacOS ***
# NOTE:
# - MacPorts:
# By default, MacPorts installs libraries under `/opt/local` (see <https://guide.macports.org>)
# Using
# % port contents qt5-qtbase
# it is found that the Qt root folder is `/opt/local/libexec/qt5`.
# - Homebrew:
# By default, Homebrew installs libraries under `/usr/local/opt/` and `/usr/local/Cellar` (see <https://docs.brew.sh/Manpage>)
# Qt default installation folder will be:
#   $HOMEBREW_PREFIX/opt/qt@5
#   $HOMEBREW_PREFIX/opt/qt5
#   $HOMEBREW_PREFIX/Cellar/qt@5/
# The variable `HOMEBREW_PREFIX` can be found via
# % brew config | grep HOMEBREW_PREFIX
# by default, HOMEBREW_PREFIX='/usr/local'.
# To obtain the value of the cofig variable use:
# % brew config | sed -nE 's;.*HOMEBREW_PREFIX:[[:blank:]]*(.+)[[:blank:]]*;\1;p'

# Qt default paths
brew1="/usr/local/opt"
brew2="/opt/homebrew/opt"
port1="/opt/local/libexec"
qt_rpaths="$brew1/qt@5/  $brew1/qt5/  $brew2/qt@5  $brew2/qt5  $port1/qt5"

HELP="* Usage: bash mk_macos_pack.sh <source-dir> <build-dir>"

# absolute source directory
if [ -z "$1" ]
then
    echo "Error: Provide the source path."
    echo "$HELP"
    exit 1
fi

pushd "$1"; source_dir=$PWD; popd

# absolute build directory
if [ -z "$2" ]
then
    echo "Error: Provide the build path."
    echo "$HELP"
    exit 1
fi
pushd "$2"; build_dir=$PWD; popd

TITLE="MacOS Package"
# turn logging off (0), log to stdout (1)
LOG_FLAG=1
LOG_TOKEN=".:"
# separator line for output
SEPLINE="=========="

#-- functions

# NOTE: `echo ... >&2` is used for logging inside functions
function log()
{
    if [[ $LOG_FLAG == 1 ]]; then
        echo "$LOG_TOKEN $1" >&2
    fi
}

# token to determine Qt dependencies
# eg. '/usr/local/opt/qt@5/lib/QtCore.framework/Versions/5/QtCore'
QT_TOKEN="/qt"

# get 1st-order dependencies for a given file
function get_depends1()
{
    local filenm=$1
    # obtain the dependencies; remove the first line which is
    # the name of the file itself; find lines containing '/opt/' or '/Cellar/'
    local optional_deps_rx='/(opt|Cellar)/'
    local _deps=$(otool -L "$filenm" | tail -n +2 | grep -E $optional_deps_rx)
    # make a clean list of dependencies without duplicates
    # otool output example: '   /usr/local/opt/foo.dylib (compatibility ...)'
    #    => '/usr/local/opt/foo.dylib'.
    local path_rx='s;[[:blank:]]*(.+)[[:blank:]]+\(.+;\1;p'
    local deps=$(echo "$_deps" | sort | uniq | sed -nE $path_rx)
    # return the dependencies
    echo $deps
}

# get dependencies up to 2nd order for a given file
function get_depends()
{
    local deps0 dep dep1 deps2 deps_all deps_all_sep

    #-- obtain the 1st-order dependencies of the given file
    deps0=$(get_depends1 "$1")

    log "* Direct dependencies of '$1':"
    for dep in $deps0
    do
        log "+ $dep"
    done

    #-- find 2nd-order dependencies
    # for each 1st-order dependency, find 1st-order dependencies
    for dep1 in $deps0
    do
        deps2=$(get_depends1 "$dep1")
        # add dependencies to the list of all dependencies
        deps_all="$deps_all  $dep1  $deps2"
    done
    log $SEPLINE

    #-- remove duplicates from the list of all dependencies (up to 2nd-order)
    # add newline between the list members since `sort` and `uniq` are line-oriented
    for dep in $deps_all
    do
        deps_all_sep="$deps_all_sep \n $dep \n"
    done
    local depends_all=$(echo -e "$deps_all_sep" | sort | uniq)
    log "* All dependencies of '$1':"
    for dep in $depends_all
    do
        log "+ $dep"
    done
    log $SEPLINE

    # returned value is stored in `depends_all`
    echo $depends_all
}


#-- package-related variables
pack_dirname="macospack"
pack_ohkl_dirname="ohkl"
pack_pyohkl_dirname="pyohkl"
pack_lib_dirname="lib"
pack_fmwk_dirname="Frameworks"
pack_qt_fmwk_dirname="Qt"
pack_filename="openhkl_macos.zip"
pack_exe_filename="OpenHKL"
pack_readme_filename="README.md"

# package structure:
# ohkl
# |
# + OpenHKL <executable>
# + dbg_macos_pkg.sh <debug script>
# + README.md
# |
# +- lib
# |  |
# |  + <external libraries, no Qt>
# |
# +- pyohkl
#    |
#    + <Python interface library>

bin_dir="main/OpenHKL.app/Contents/MacOS"
ohkl_bin0="$build_dir/$bin_dir/$pack_exe_filename"
root_dir="$build_dir/$pack_dirname"
pack_dir="$root_dir/$pack_ohkl_dirname"
pyohkl_dir="$pack_dir/$pack_pyohkl_dirname"
xlib_dir="$pack_dir/$pack_lib_dirname"
fmwk_dir="$pack_dir/$pack_fmwk_dirname"
qt_fmwk_dir="$fmwk_dir/$pack_qt_fmwk_dirname"
dbg_script="$source_dir/setup_scripts/dbg_macos_pkg.sh"
pkg_intro="$source_dir/setup_scripts/intro_macos_pkg.md"

ohkl_bin="$pack_dir/OpenHKL"
ohkl_readme="$pack_dir/$pack_readme_filename"
pyohkl_files0=$(ls -Ap $build_dir/swig/*.{so,py})
pyohkl_lib0="$build_dir/swig/_pyohkl.so"

# arrays to store dependency data
declare -a depends_fullpath=( ) depends_filename=( ) \
   qt_depends_fullpath=( ) qt_depends_filename=( )

# start from the dependencies of the binary
deps_paths=$(get_depends "$ohkl_bin0")

# collect the dependency data into arrays

qt_fmwk_rx='s;(.+)/lib/(.+);lib/\2;'
for dep in $deps_paths
do
    fnm=$(basename "$dep")  # dependency basename
    # Qt dependencies are treated differently due to their framework structure
    if [[ "$dep" == *"$QT_TOKEN"* ]]; then
        qt_depends_fullpath+=( $dep )
        # eg. '/usr/local/opt/qt@5/lib/QtCore.framework/Versions/5/QtCore' => 'lib/QtCore.framework/Versions/5/QtCore'
        fmwkdir=$(echo "$dep" | sed -E $qt_fmwk_rx)
        qt_depends_filename+=( $fmwkdir )
    else
        depends_fullpath+=( $dep )
        depends_filename+=( $fnm )
    fi
done

# Qt directory on the build machine
qt_dir_rx='s;(.+)/lib/(.+);\1;'
# eg. '/usr/local/opt/qt@5/lib/QtCore.framework/Versions/5/QtCore' => '/usr/local/opt/qt@5'
qt_dir_build=$(echo "${qt_depends_fullpath[0]}" | sed -E $qt_dir_rx)
qt_rpaths="$qt_rpaths $qt_dir_build"

# extract the Python dependency from the pyohkl library
# regexp to extract the Python dependence
py_fmwk_rx='s;[[:blank:]]*(.+)/(Python|libpython.+\.dylib).*;\1/\2;p'
# regexp to extract the Python framework path
# eg.: '/usr/local/opt/python@3.9/Frameworks/Python.framework/Versions/3.9/Python'
#   => '/usr/local/opt/python@3.9/Frameworks/Python.framework/Versions/3.9/'
py_fmwk_path_rx='s;(.*)/(Python|libpython).*;\1;'
# regexp to correct the Python dependence; eg.:
# '/usr/local/opt/python@3.9/Frameworks/Python.framework/Versions/3.9/Python' => 'libpython3.9.dylib'
# '/usr/local/opt/python@3.9/Frameworks/Python.framework/Versions/3.9/libpython3.9.dylib' => 'libpython3.9.dylib'
pylib_rx='s;.*[pP]ython.+[Vv]ersions/([0-9.]+).+(Python|libpython).*;libpython\1.dylib;'
# regexp to extract the Python version
pyversion_rx='s;.*[pP]ython.+[Vv]ersions/([0-9.]+).*;\1;'

pydeps0=$(otool -L "$pyohkl_lib0")
pydepends_fullpath=$(echo "$pydeps0" | sed -nE $py_fmwk_rx)
pydepends_filename=$(echo "$pydepends_fullpath" | sed -E $pylib_rx)
pyversion=$(echo "$pydepends_fullpath" | sed -E $pyversion_rx)
py_fmwk_dir=$(echo "$pydepends_fullpath" | sed -E $py_fmwk_path_rx)

# add RPATHs corresponding to the common framework paths
framework_paths="/usr/local/Library/Frameworks  /Library/Frameworks  /usr/local/Frameworks"
py_fmwk_path="Python.framework/Versions/$pyversion/lib"

for pth in $framework_paths; do
    py_fmwk_rpaths="$py_fmwk_rpaths  $pth/$py_fmwk_path"
done

#-- make the package directory structure
# make the dependency folders

MKDIR="mkdir -vp"
COPY="cp -nv"

# copy the executable
echo "$TITLE: Copy executable..."
$MKDIR "$pack_dir"
$COPY "$ohkl_bin0" "$ohkl_bin"
exefiles="$ohkl_bin"

# copy the debug script
echo "$TITLE: Copy debug script..."
$COPY "$dbg_script" "$pack_dir"

# copy the external libraries (excluding Qt)
echo "$TITLE: Copy external dependencies to '$xlib_dir'..."
$MKDIR "$xlib_dir"
for dep in "${depends_fullpath[@]}";
do
    $COPY "$dep" "$xlib_dir"
done
# list of copied files
xlibfiles=$(ls -Ap "$xlib_dir/"* | grep -v /$)

# copy the Python library
echo "$TITLE: Copy Python library '$pyohkl_dir'..."
$MKDIR "$pyohkl_dir"

for dep in $pyohkl_files0;
do
    $COPY "$dep" "$pyohkl_dir/"
done
pyohkl_files=$(ls -Ap "$pyohkl_dir/"* | grep -v /$)
pyohkl_lib=$(ls -Ap "$pyohkl_dir"/*.so)

#-- modify the library references in all files
# ref to external libraries
for filenm in $xlibfiles $ohkl_bin $pyohkl_lib
do
    for idx in "${!depends_fullpath[@]}";
    do
        pth=${depends_fullpath[idx]}
        fnm=${depends_filename[idx]}
        # change the dependency path in the library
        # eg. '/usr/local/opt/foo.dylib' => '@rpath/foo.dylib'
        rpth="@rpath/$fnm"
        install_name_tool "$filenm" -change "$pth" "$rpth"
    done

    log "$TITLE: Changed references in '$filenm'"
done
# ref to Qt libraries
# NOTE: It is assumed that only the executable contains Qt dependencies
for filenm in $ohkl_bin
do
    for idx in "${!qt_depends_fullpath[@]}";
    do
        pth=${qt_depends_fullpath[idx]}
        fnm=${qt_depends_filename[idx]}
        # change the corresponding external library path in the library
        # eg. '/usr/local/opt/qt@5/lib/QtCore.framework/Versions/5/QtCore' => '@rpath/lib/QtCore.framework/Versions/5/QtCore'
        rpth="@rpath/$fnm"
        install_name_tool "$filenm" -change "$pth" "$rpth"
    done

    log "$TITLE: Changed Qt references in '$filenm'"
done
#ref to Python library
install_name_tool "$pyohkl_lib" -change "$pydepends_fullpath" "@rpath/$pydepends_filename"

#-- add proper RPATHs

# add RPATHs to libraries
for fnm in $xlibfiles
do
    install_name_tool "$fnm" -add_rpath "@loader_path"
done

# add RPATHs to executable
install_name_tool "$ohkl_bin" -add_rpath "@loader_path/$pack_lib_dirname"
for rpth in $qt_rpaths
do
    install_name_tool "$ohkl_bin" -add_rpath "$rpth"
done

# add RPATHs to the Python library
install_name_tool "$pyohkl_lib" \
   -add_rpath "@loader_path/../$pack_lib_dirname" \
   -add_rpath "$py_fmwk_dir/lib"

for rpth in $py_fmwk_rpaths; do
    install_name_tool "$pyohkl_lib" -add_rpath "$rpth"
done

#-- make a README to include instructions and details
machine_specs=$(uname -msr)
git_branch=$(git branch --show-current)
git_commit=$(git rev-parse --short HEAD)
date_stamp=$(date "+%d-%m-%Y,%H:%M")
dyld_debug_cmd="DYLD_PRINT_LIBRARIES=1 DYLD_PRINT_RPATHS=1 DYLD_PRINT_TO_FILE=dyld.log ./$pack_exe_filename"

echo "# OpenHKL MacOS zip-package:" > $ohkl_readme
echo "- Build directory = '$build_dir'" >> $ohkl_readme
echo "- OpenHKL package file = '$pack_filename'" >> $ohkl_readme
echo "- Machine specifications: $machine_specs" >> $ohkl_readme
echo "- Git repository branch '$git_branch' at $git_commit" >> $ohkl_readme
echo "- Package created on $date_stamp" >> $ohkl_readme
# show the contents of the README file
cat "$ohkl_readme"

# add instructions
cat "$pkg_intro" >> $ohkl_readme
echo >> $ohkl_readme

# add details
echo "* Details:" >> $ohkl_readme
echo "- Executables:" >> $ohkl_readme
echo "$ohkl_bin" >> $ohkl_readme
echo >> $ohkl_readme

echo "- Python library:" >> $ohkl_readme
idx=0
for fnm in $pyohkl_files
do
    idx=$((idx+1))
    printf "%3d) %s" "$idx" "$fnm\n" >> $ohkl_readme
done
echo >> $ohkl_readme

echo "- Library files:" >> $ohkl_readme
idx=0
for fnm in $xlibfiles
do
    idx=$((idx+1))
    printf "%3d) %s" "$idx" "$fnm\n" >> $ohkl_readme
done
echo >> $ohkl_readme

echo "- Library reference changes:" >> $ohkl_readme
for idx in "${!depends_fullpath[@]}"
do
    pth=${depends_fullpath[idx]}
    fnm=${depends_filename[idx]}
    printf "%3d) '%s' => '%s'\n" "$idx" "$pth" "$fnm" >> $ohkl_readme
done
echo >> $ohkl_readme

echo "- Qt reference changes:" >> $ohkl_readme
for idx in "${!qt_depends_fullpath[@]}"
do
    pth=${qt_depends_fullpath[idx]}
    fnm=${qt_depends_filename[idx]}
    printf "%3d) '%s' => '%s'\n" "$idx" "$pth" "$fnm" >> $ohkl_readme
done
echo >> $ohkl_readme

echo "- RPATHs:" >> $ohkl_readme
echo "@loader_path" >> $ohkl_readme
echo "@loader_path/$pack_lib_dirname" >> $ohkl_readme
echo "@loader_path/../$pack_lib_dirname" >> $ohkl_readme
echo "$py_fmwk_dir/lib" >> $ohkl_readme
for rpth in $py_fmwk_rpaths; do
    echo "$rpth" >> $ohkl_readme
done
for rpth in $qt_rpaths
do
    echo "$rpth" >> $ohkl_readme
done
echo >> $ohkl_readme

#-- make a zip-package
echo "$TITLE: Building package '$pack_filename'..."
pushd "$root_dir"
zip -v9r "$pack_filename" "$pack_ohkl_dirname/"
popd

#-- final message
echo "$TITLE: Done."
