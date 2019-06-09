#include <fstream>
#include <iostream>
#include <string>

std::string load_file(std::ifstream& fin)
{
    std::string contents;
    fin.seekg(0, std::ios::end);
    contents.reserve(fin.tellg());
    fin.seekg(0, std::ios::beg);
    contents.assign((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());
    fin.close();

    return contents;
}

int main(int argc, char** argv)
{
    if (argc < 4) {
        std::cerr << "USAGE: " << argv[0]
                  << "{res type} {res name} {res map cpp}\n\nUpdates the {res map "
                     "cpp}.cpp file using the {res type} and {res name} of the "
                     "resource\n";
        return 1;
    }

    std::string resource_type(argv[1]);
    std::string resource_name(argv[2]);
    std::string resources_map_cpp_filename(argv[3]);

    // Read the current contents of the resources cpp file and insert the YAML
    // contents
    std::ifstream fin(resources_map_cpp_filename.c_str());
    auto contents = load_file(fin);
    fin.close();

    auto pos = contents.rfind("};");
    contents.insert(
        pos,
        "{{\"" + resource_type + "\",\"" + resource_name + "\"},&resource_" + resource_type + "_"
            + resource_name + "},\n");

    pos = contents.rfind("namespace nsx {");
    contents.insert(pos - 1, "#include \"SingleResource" + resource_name + ".h\"\n");

    std::ofstream fout(resources_map_cpp_filename.c_str());
    fout << contents;
    fout.close();
}
