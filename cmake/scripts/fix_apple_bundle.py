"""
Script to fix all dependencies in OS X bundle. Runs automatically when -DENABLE_CPACK is set to ON in a MacOS environment
"""
from __future__ import print_function
import os
import sys
import platform
import shutil
import subprocess
import glob
from distutils import sysconfig

BUNDLE_DIR = ""
PYTHON_LIB = ""

# -----------------------------------------------------------------------------
# Locations
# -----------------------------------------------------------------------------

def is_python3():
    if (sys.version_info > (3, 0)):
        return True
    else:
        return False


def python_version_string():
    return str(sys.version_info[0]) + "." + str(sys.version_info[1])


def set_python_library(python_lib_dir):
    global PYTHON_LIB
    PYTHON_LIB = os.path.realpath(python_lib_dir)


def set_bundle_dir(bundle_dir):
    global BUNDLE_DIR
    BUNDLE_DIR = bundle_dir


def bundle_dir():
    global BUNDLE_DIR
    return BUNDLE_DIR


def bundle_frameworks_path():
    return os.path.join(bundle_dir(), "Contents", "Frameworks")


def bundle_lib_path():
    return os.path.join(bundle_dir(), "Contents", "lib")


def bundle_macos_path():
    return os.path.join(bundle_dir(), "Contents", "MacOS")


def bundle_plugins_path():
    return os.path.join(bundle_dir(), "Contents", "PlugIns")


def bundle_main_executables():
    return [os.path.join(bundle_dir(), "Contents", "MacOS", "nsxqt")]


def bundle_python_library():
    return os.path.join("Python.framework", "Versions", python_version_string(), "Python")


def qtlibs_path():
    return os.path.join(os.environ['QTDIR'], "lib")


def qtplugins_path():
    return os.path.join(os.environ['QTDIR'], "plugins")


def bundle_libraries():
    bundle_contents_lib = os.path.join(bundle_dir(), "Contents","lib")
    return [os.path.join(bundle_contents_lib, "libnsx.dylib"),os.path.join(bundle_contents_lib,"python%s"%python_version_string(),"site-packages","_pynsx.so")]


def bundle_plugins():
    return glob.glob(os.path.join(bundle_plugins_path(), "*", "*"))


def nsxtool_binaries():
    return bundle_main_executables() + bundle_libraries() + bundle_plugins()


def get_list_of_files(dirname):
    """
    returns recursive list of files in given directory and its subdirectories
    """
    result = []
    for root, directories, filenames in os.walk(dirname):
        for filename in filenames:
            result.append(os.path.join(root,filename))
    return result


# -----------------------------------------------------------------------------
# Utils
# -----------------------------------------------------------------------------


def make_dir(dirname):
    if not os.path.exists(dirname):
        os.makedirs(dirname)


def copy_file_to_dir(file_name, dest_dir):
    """
    Copies file to the destination directory. If destination doesn't exists, it will be created.
    """
    make_dir(dest_dir)
    dest_file = os.path.join(dest_dir, os.path.basename(file_name))
    shutil.copyfile(file_name, dest_file)
    return dest_file


def copy_file_to_file(source, destination):
    """
    Copies file to another. If destination directory doesn't exists, it will be created.
    """
    make_dir(os.path.dirname(destination))
    shutil.copyfile(source, destination)


def otool(filename):
    """
    Parses dependencies of given binary file
    """
    p = subprocess.Popen(['otool', '-XL', filename], stdout=subprocess.PIPE)
    # return iter(p.stdout.readline, b'')
    for line in iter(p.stdout.readline, b''):
        if is_python3():
            yield line.decode('ascii').strip().split()[0]
        else:
            yield line.strip().split()[0]


def setId(filename, newId):
    """
    Sets new id for binary file
    """
    p = subprocess.Popen(['install_name_tool', '-id', newId, filename], stdout=subprocess.PIPE)
    p.communicate()
    return


def fixDependency(filename, old, new):
    """
    Replaces old dependency with new one for given binary file
    """
    print("    fixDependency(filename, old, new)", filename, old, new)
    p = subprocess.Popen(['install_name_tool', '-change', old, new, filename], stdout=subprocess.PIPE)
    p.communicate()
    return


def is_system_dependency(dependency):
    """
    Returns True if this is system file dependency
    """
    non_dependency_patterns = ['/usr/lib', '/System']
    for pat in non_dependency_patterns:
        if dependency.startswith(pat):
            return True
    return False


def is_rpath_dependency(dependency):
    """
    Returns True if this is rpath like dependency
    """
    non_dependency_patterns = ['@']
    for pat in non_dependency_patterns:
        if dependency.startswith(pat):
            return True
    return False


def is_python_framework_dependency(dependency):
    """
    Returns True if this dependency is python library
    """
    if not "boost" in dependency.lower():
        if "python" in dependency.lower():
            return True
    return False


def is_qt_framework_dependency(dependency):
    """
    Returns True if this dependency is Qt related
    """
    if "Qt" in dependency:
        return True
    return False


def is_to_bundle_dependency(dependency):
    """
    Returns True if this dependency should be moved to the bundle.
    Qt libraries and Python framework are special case and will be treated separately.
    """

    if is_system_dependency(dependency):
        return False

    if "libnsx" in dependency:
        # our own libraries are already in place and have right libId's
        return False

    return True


def get_dependency_libId(dependency):
    """
    Returns libId of this dependency
    """
    libname = os.path.basename(dependency)

    if is_python_framework_dependency(dependency):
        return "@rpath/" + bundle_python_library()

    if is_qt_framework_dependency(dependency):
        return "@rpath/" + libname +".framework/Versions/5/"+libname

    # all other libraries
    return "@rpath/" + libname


def get_dependency_orig_location(dependency):
    """
    Returns dependency original location. In most cases it is just "dependency" itself (i.e. path to file).
    If dependency contains @rpath, then we are trying to guess where original files might be
    """

    if os.path.exists(dependency):
        return dependency

    result = None

    if is_python_framework_dependency(dependency):
        result = get_python_library_location()

    elif is_qt_framework_dependency(dependency):
        libname = os.path.basename(dependency)
        libpath = os.path.join(libname+".framework", "Versions", "5")
        result = os.path.join(qtlibs_path(), libpath, libname)

    return result


def get_dependency_dest_location(dependency):
    """
    Return new location of the dependency in the bundle
    """
    libname = os.path.basename(dependency)

    if is_python_framework_dependency(dependency):
        return os.path.join(bundle_frameworks_path(), bundle_python_library())

    if is_qt_framework_dependency(dependency):
        libpath = os.path.join(libname+".framework", "Versions", "5")
        return os.path.join(bundle_frameworks_path(), libpath, libname)

    return os.path.join(bundle_frameworks_path(), libname)


def get_special_dependency_id(dependency):
    """
    Returns libId if this dependency requires special attention (i.e. Python framework)
    """
    if is_python_framework_dependency(dependency):
        return "@rpath/" + bundle_python_library()

    if is_qt_framework_dependency(dependency) and not "@rpath" in dependency:
        libname = os.path.basename(dependency)
        return "@rpath/" + libname +".framework/Versions/5/"+libname
    return None


def get_python_library_location():
    """
    Returns location of Python library. The library is deduced from interpreter itself
    """
    for dependency in otool(sys.executable):
        print(dependency)
        if os.path.exists(dependency) and "Python.framework" in dependency:
            return dependency

    # At this point it looks that interpreter depends from libPython via @loader_path
    # Let's try to find library directly

    prefix = sys.prefix
    suffix = sysconfig.get_config_var('LDVERSION') or sysconfig.get_config_var('VERSION')
    result = sys.prefix+"/lib/libpython"+suffix+".dylib"
    if os.path.exists(result):
        return result

    return None


# -----------------------------------------------------------------------------
# Actions
# -----------------------------------------------------------------------------


def copy_python_framework():
    """
    Copies Python library to the bundle. The name of the library will be deduced from the interpreter itself.
    """
    print("--> Copying Python framework")
    python_lib = get_python_library_location()
    destfile = os.path.join(bundle_frameworks_path(), bundle_python_library())
    make_dir(os.path.dirname(destfile))
    print("    From '{0}'\n    To '{1}'".format(python_lib, destfile))
    if not os.path.exists(destfile):
        shutil.copyfile(python_lib, destfile)
        libId = "@rpath/" + bundle_python_library()
        setId(destfile, libId)


def copy_qt_libraries():
    print("--> Copying Qt libraries")
    libs = ['QtCore', 'QtDBus', 'QtDesigner', 'QtGui', 'QtPrintSupport', 'QtWidgets', 'QtXml', 'QtSvg', 'QtNetwork', 'QtOpenGL']
    print("   ", end="")
    for libname in libs:
        print(libname, end="")
        libpath = os.path.join(libname+".framework", "Versions", "5")
        srcfile = os.path.join(qtlibs_path(), libpath, libname)
        if os.path.exists(srcfile):
            dstdir = os.path.join(bundle_frameworks_path(), libpath)
            copy_file_to_dir(srcfile, dstdir)


def copy_qt_plugins():
    print("--> Copying Qt plugins")
    plugins = ['platforms/libqcocoa.dylib', 'iconengines/libqsvgicon.dylib',
        'imageformats/libqjpeg.dylib', 'imageformats/libqsvg.dylib']
    print("   ", end="")
    for name in plugins:
        print(name, end="")
        srcfile = os.path.join(qtplugins_path(), name)
        dstdir = os.path.join(bundle_plugins_path(), os.path.dirname(name))
        dstfile = copy_file_to_dir(srcfile, dstdir)
        setId(dstfile, os.path.basename(dstfile))


def process_dependency(dependency):
    """
    Copies external dependency library into Frameworks directory (if it is not already there)
    and sets corresponding library Id for it. Returns library Id and new location to the user.
    """

    libId = get_dependency_libId(dependency)
    origLocation  = get_dependency_orig_location(dependency)
    destLocation = get_dependency_dest_location(dependency)

    print("     ------")
    if libId == dependency:
        print("     selfDependency")
        return None, None

    if origLocation == None:
        print("     origLocation >", origLocation)
        return libId, None

    print("     origLocation >", origLocation)
    print("     destLocation >", destLocation)
    print("            libId >", libId)

    if os.path.exists(destLocation):
        return libId, None
    print("       copying to >", destLocation)
    copy_file_to_file(origLocation, destLocation)
    setId(destLocation, libId)
    return libId, destLocation


def walk_through_dependencies(file_name):
    print("============================")
    print("walk_through ", file_name)
    print(list(otool(file_name)))
    for dependency in otool(file_name):
        if dependency[-1] == ":":
            dependency = dependency[:-1]
        print("---> ", file_name, dependency)
        if is_to_bundle_dependency(dependency):
            libId, new_location = process_dependency(dependency)
            print("to Bundle", libId, new_location)
            if libId:
                fixDependency(file_name, dependency, libId)
                if new_location:
                    walk_through_dependencies(new_location)


def copy_dependencies():
    print("--> Copying third party dependencies")
    for binfile in iter(nsxtool_binaries()):
        walk_through_dependencies(binfile)


def validate_dependencies():
    """
    Analyse whole bundle for missed dependencies
    """
    binaries = nsxtool_binaries()
    libraries = get_list_of_files(bundle_frameworks_path())
    file_list = binaries+libraries
    files_with_missed_dependencies = []
    for file_name in file_list:
        for dependency in otool(file_name):
            if is_system_dependency(dependency) or is_rpath_dependency(dependency):
                continue
            if os.path.basename(file_name) in dependency:
                # self dependency (libId)
                continue
            files_with_missed_dependencies.append(file_name)
            break

    if len(files_with_missed_dependencies):
        print("Error! Still unresolved dependencies.")
        print(files_with_missed_dependencies)
        raise Exception("Unresolved dependencies!")

def bundle_python():

    from distutils.dir_util import copy_tree

    pythonxy = "python%s" % python_version_string()

    site_packages = os.path.join(bundle_lib_path(),pythonxy,"site-packages")

    # Copy the python dylib and set its id
    python_dylib = os.path.join(bundle_lib_path(),"lib%s.dylib" % pythonxy)
    shutil.copy(PYTHON_LIB,python_dylib)
    os.chmod(python_dylib,0777)
    setId(python_dylib,"@loader_path/%s" % os.path.basename(python_dylib))    

    # Copy the Python exectubale and fix its dependency on python lib
    python_exe = os.path.join(bundle_macos_path(),"python")
    shutil.copy(sys.executable,python_exe)
    fixDependency(python_exe,PYTHON_LIB,"@executable_path/../lib/%s" % os.path.basename(python_dylib))

    # Copy the python lib directory contents
    shutil.move(os.path.join(site_packages,"_pynsx.so"),bundle_lib_path())
    shutil.move(os.path.join(site_packages,"pynsx.py"),bundle_lib_path())
    copy_tree(os.path.join(os.path.dirname(PYTHON_LIB),"lib",pythonxy),os.path.join(bundle_lib_path(),pythonxy))
    shutil.rmtree(site_packages)
    make_dir(site_packages)
    shutil.move(os.path.join(bundle_lib_path(),"_pynsx.so"),site_packages)
    shutil.move(os.path.join(bundle_lib_path(),"pynsx.py"),site_packages)

    # Add NumPy to the bundled python
    numpy_dir = os.path.join(site_packages,"numpy")
    import numpy
    shutil.copytree(numpy.__path__[0],numpy_dir)

    fixDependency(os.path.join(site_packages,"_pynsx.so"),"@rpath/Python.framework/Versions/2.7/Python","@loader_path/../../lib%s.dylib" % pythonxy)


def fix_apple_bundle():
    print('-'*80)
    print("Fixing OS X bundle at '{0}'".format(bundle_dir()))
    print('-'*80)
    copy_python_framework()
    # FIXME provide automatic recognition of Qt dependency type (@rpath or hard coded)
    #copy_qt_libraries() # this line should be uncommented for macport based builds
    copy_qt_plugins()
    copy_dependencies()
    validate_dependencies()
    print("Done!")


if __name__ == '__main__':
    if not platform.system() == 'Darwin':
        exit("This script is intended for MacOs systems. Exiting...")

    if len(sys.argv) != 3:
        exit("Please specify bundle location")

    set_bundle_dir(sys.argv[1])

    set_python_library(sys.argv[2])

    fix_apple_bundle()

    bundle_python()

