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
    if (argc < 4) {
        std::cerr<<"USAGE: "<<argv[0]<<"{res file} {res name} {res header}\n\nUpdate {res header}.h from the contents of {res file} using {res name} key\n";
        return 1;
    }

    // Read the contents of the YAML file
    std::string instrument(argv[1]);
    std::ifstream fin(instrument.c_str());
    if (!fin.is_open()) {
        std::cerr<<"The file "<<instrument<<" could not be opened\n";
        return 1;
    }
    auto yaml_contents = load_file(fin);
    fin.close();

    std::string resource_name(argv[2]);

    // Read the current contents of the resources cpp file and insert the YAML contents
    std::string resource_header_filename(argv[3]);
    std::ifstream resources_header_in(resource_header_filename.c_str());
    auto header_contents = load_file(resources_header_in);
    auto pos = header_contents.rfind("};");
    header_contents.insert(pos,"{\""+resource_name+"\",R\"(" + yaml_contents + ")\"},");
    resources_header_in.close();

    std::ofstream resources_header_out(resource_header_filename.c_str());
    resources_header_out << header_contents;
    resources_header_out.close();
}
