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

#ifndef D2D_CMD_PARSE_H
#define D2D_CMD_PARSE_H

#include <vector>
#include <string>
#include "export.h"

class cmd_opt {
private:
    enum CMD_MODE {
        CMD_OPT,
        CMD_OUTPUT_ARG,
        CMD_THRESHOLD_ARG,
        CMD_DEPTH_ARG,
        CMD_MAX_SUBNODES_ARG,
        CMD_EXPORT_ARG,
        CMD_NODE_ARG,
        CMD_LABEL_ARG
    };
public:
    struct NodePath {
        std::vector<std::string> node;
        std::string literal;  /* for error report */
    };
    std::string ifile;
    std::string ofile;
    double threshold;
    int depth;
    int max_subnodes;
    bool critical_only;
    enum ExportType export_type;
    std::vector<NodePath> nodes;
    std::vector<uintptr_t> labels;

    cmd_opt() : threshold(0), critical_only(false), depth(-1), max_subnodes(-1), export_type(EXPORT_DOT) {}
    std::string help(const char* app);
    void parse_node(const char *text);
    int parse(int argc, char **argv);
};

#endif //D2D_CMD_PARSE_H
