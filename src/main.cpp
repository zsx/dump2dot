#include <iostream>
#include "cmd_parse.h"
#include "dump.h"

int main(int argc, char **argv)
{
    cmd_opt opt;
    try {
        auto ret = opt.parse(argc, argv);
        if (ret < 0) {
            std::cout << "Wrong command line" << std::endl;
            std::cout << opt.help(argv[0]);
            //			return EXIT_FAILURE;
        }
        else if (ret > 0) {
            std::cout << opt.help(argv[0]);
            return EXIT_SUCCESS;
        }
    }
    catch (...) {
        std::cout << "Exception happened in command parse" << std::endl;
        opt.help(argv[0]);
        return EXIT_FAILURE;
    }

    if (opt.ofile.empty()) {
        opt.ofile = "a.dot";
    }

    std::cout.imbue(std::locale(""));

    try {
        MemoryDump dump;
        if (!dump.import(opt.ifile)) {
            std::cout << "Failed to parse the input" << std::endl;
        }
        dump.update_subtree_size();
        dump.write_output(opt);
    }
    catch (...) {
        std::cout << "Unexpected error hanppend" << std::endl;
    }

    return EXIT_SUCCESS;
}
