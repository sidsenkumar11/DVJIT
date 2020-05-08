#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>
#include <vector>

#include "helpers.hpp"
#include "jit_compile.hpp"

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

    // read the file in to a std::stringstream, which is the
    // format most convenient for jit_compile i(robert) think...
    std::ifstream file(argv[0]);
    if (file)
    {
        // taken from https://stackoverflow.com/a/138645
        file.seekg(0,std::ios::end);
        std::streampos          length = file.tellg();
        file.seekg(0,std::ios::beg);

        std::vector<char>       buffer(length);
        file.read(&buffer[0],length);
        
        std::stringstream       localStream;
        localStream.rdbuf()->pubsetbuf(&buffer[0],length);

        // jit compile the code!!!! 
        jit_result *p = jit_compile(localStream);

        if (p != nullptr)
        {
            // p->ptr();
            jit_release(p);
        }
    }


    return 0;
}
