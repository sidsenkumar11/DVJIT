#include <fstream>
#include <iostream>
#include <cstring>

#include "helpers.hpp"

using namespace std;


int main(int argc, char **argv)
{
    if (argc < 2)
    {
        cerr << "Supply file name" << endl;
        return 1;
    }
    if (argc > 2)
    {
        cerr << "Too many arguments" << endl;
        return 1;
    }

    if (is_debug())
    {
        cout << "Reading file: " << argv[0] << endl;
    }

    // read the file in
    ifstream ifs(argv[0]);
    string content( (std::istreambuf_iterator<char>(ifs) ),
                    (std::istreambuf_iterator<char>()    ));


    return 0;
}
