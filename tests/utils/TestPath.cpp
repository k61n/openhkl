#include <string>

#include <core/NSXTest.h>
#include <core/Path.h>
#include <core/StringIO.h>

NSX_INIT_TEST

int main(int argc, char** argv)
{
    // Test nsx::fileSeparator
    auto file_sep = nsx::fileSeparator();
    std::string path = file_sep + "usr" + file_sep + "local" + file_sep + "file.txt";
    #ifdef _WIN32
        NSX_CHECK_EQUAL(path,"\\usr\\local\\file.txt");
    #else
        NSX_CHECK_EQUAL(path,"/usr/local/file.txt");
    #endif

    // Test nsx::buildPath for bulding a path from a vector of string
    std::vector<std::string> path_components({file_sep+"usr","local","file.txt"});
    #ifdef _WIN32
        NSX_CHECK_EQUAL(nsx::buildPath(path_components,""),path);
    #else
        NSX_CHECK_EQUAL(nsx::buildPath(path_components,""),path);
    #endif

    // Test nsx::splitFileExtension for splitting a filename from its filename and its extension
    // Case where the filename has an extension
    auto ext_splitted_filename = nsx::splitFileExtension(path);
    #ifdef _WIN32
        NSX_CHECK_EQUAL(ext_splitted_filename.first,"\\usr\\local\\file");
    #else
        NSX_CHECK_EQUAL(ext_splitted_filename.first,"/usr/local/file");
    #endif
    NSX_CHECK_EQUAL(ext_splitted_filename.second,".txt");

    // Test nsx::splitFileExtension for splitting a filename from its filename and its extension
    // Case where the filename has no extension
    ext_splitted_filename = nsx::splitFileExtension(ext_splitted_filename.first);
    #ifdef _WIN32
        NSX_CHECK_EQUAL(ext_splitted_filename.first,"\\usr\\local\\file");
    #else
        NSX_CHECK_EQUAL(ext_splitted_filename.first,"/usr/local/file");
    #endif
    NSX_CHECK_EQUAL(ext_splitted_filename.second,"");

    // Test nsx::fileBasename function for getting the basename from a path
    auto basename = nsx::fileBasename(path);
    NSX_CHECK_EQUAL(basename,"file.txt");

    // Test nsx::fileDirname function for getting the directory name from a path
    auto dirname = nsx::fileDirname(path);
    #ifdef _WIN32
        NSX_CHECK_EQUAL(dirname,"\\usr\\local");
    #else
        NSX_CHECK_EQUAL(dirname,"/usr/local");
    #endif

    // Test nsx::fileExists for checking whether a file exists or not
    NSX_CHECK_ASSERT(nsx::fileExists(argv[0]));
    NSX_CHECK_ASSERT(!nsx::fileExists("i-do-not-exist"));

}
