#include <fstream>
#include <iostream>
#include <string>

std::string load_file(std::ifstream& fin)
{
    std::string contents;
    fin.seekg(0, std::ios::end);
    contents.reserve(fin.tellg());
    fin.seekg(0, std::ios::beg);
    contents.assign((std::istreambuf_iterator<char>(fin)),std::istreambuf_iterator<char>());
    fin.close();

    return contents;
}

int main(int argc, char** argv)
{
    if (argc < 3) {
        std::cerr<<"USAGE: "<<argv[0]<<"{res file} {res cpp}\n\nSet the {res cpp}.cpp file using the contents of {res file}file\n";
        return 1;
    }

    std::string resource_filename(argv[1]);
    std::string resource_cpp_filename(argv[2]);

    // Read the contents of the YAML file
    std::ifstream fin(resource_filename.c_str());
    if (!fin.is_open()) {
        std::cerr<<"The file "<<resource_filename<<" could not be opened\n";
        return 1;
    }
    auto yaml_contents = load_file(fin);
    fin.close();

    // Read the current contents of the resources cpp file and insert the YAML contents
    std::ifstream resources_cpp_in(resource_cpp_filename.c_str());
    auto cpp_contents = load_file(resources_cpp_in);
    auto pos = cpp_contents.find("data(");
    cpp_contents.insert(pos+5,"R\"(" + yaml_contents + ")\"");
    resources_cpp_in.close();

    std::ofstream resources_cpp_out(resource_cpp_filename.c_str());
    resources_cpp_out << cpp_contents;
    resources_cpp_out.close();
}
