/*
**  Copyright 2016 Atronix Engineering, Inc
**
**  Licensed under the Apache License, Version 2.0 (the "License");
**  you may not use this file except in compliance with the License.
**  You may obtain a copy of the License at
**
**  http://www.apache.org/licenses/LICENSE-2.0
**
**  Unless required by applicable law or agreed to in writing, software
**  distributed under the License is distributed on an "AS IS" BASIS,
**  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**  See the License for the specific language governing permissions and
**  limitations under the License.
*/

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
