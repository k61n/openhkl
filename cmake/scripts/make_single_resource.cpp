#include <fstream>
#include <iostream>
#include <string>
#include <vector>

std::vector<std::string> load_file(std::ifstream& fin)
{
    std::vector<std::string> contents;
    while (fin.good()) {
        std::string line;
        getline(fin, line);
        if (fin.eof()) {
            break;
        }
        contents.push_back(line);
    }

    return contents;
}

int main(int argc, char** argv)
{
    if (argc < 3) {
        std::cerr << "USAGE: " << argv[0]
                  << "{res file} {res cpp}\n\nSet the {res cpp}.cpp file using the "
                     "contents of {res file}file\n";
        return 1;
    }

    std::string resource_filename(argv[1]);
    std::string resource_cpp_filename(argv[2]);

    // Read the contents of the YAML file
    std::ifstream fin(resource_filename.c_str());
    if (!fin.is_open()) {
        std::cerr << "The file " << resource_filename << " could not be opened\n";
        return 1;
    }
    auto yaml_contents = load_file(fin);
    fin.close();

    // Read the current contents of the resources cpp file and insert the YAML
    // contents
    std::ifstream resources_cpp_in(resource_cpp_filename.c_str());
    auto cpp_contents = load_file(resources_cpp_in);
    resources_cpp_in.close();
    for (size_t i = 0; i < cpp_contents.size(); ++i) {
        auto pos = cpp_contents[i].find("static std::vector<std::string> data = {");
        if (pos != std::string::npos) {
            for (auto it = yaml_contents.rbegin(); it != yaml_contents.rend(); it++) {
                cpp_contents.insert(cpp_contents.begin() + i + 1, "\t{\"" + *it + "\"},");
            }
            break;
        }
    }

    std::ofstream resources_cpp_out(resource_cpp_filename.c_str());
    for (auto line : cpp_contents) {
        resources_cpp_out << line << std::endl;
    }
    resources_cpp_out.close();
}
