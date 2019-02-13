#include <sstream>
#include <string>
#include <vector>

#include <core/CSV.h>
#include <core/NSXTest.h>

NSX_INIT_TEST

int main() {
    std::string csv_file =
        //    ",,,\n"
        //    "a, b, c, d\n"
        //    "e, f, g, h\n"
        //    "1, 2, 3, 4\n"
            "\"\", \"\"\"\"\", \"as,df\", \" \"\"\" \"";

    std::istringstream stream(csv_file);
    nsx::CSV csv;
    std::vector<std::string> row;

    while(!stream.eof()) {
        row = csv.getRow(stream);
        NSX_CHECK_ASSERT(row.size() == 4);
    }

    return 0;
}
