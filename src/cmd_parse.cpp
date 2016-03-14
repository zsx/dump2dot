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

#include <sstream>

#include "cmd_parse.h"
#include <cstring>
#include <iostream>
#include <string.h>

#ifdef _MSC_VER
#define strcasecmp _stricmp
#endif

std::string cmd_opt::help(const char* app)
{
    std::stringstream ss;
    ss << "Usage:" << std::endl
        << app << " [options] input" << std::endl
        << "-h, --help\t\t\tThis help" << std::endl
        << "-o, --output\tfile\t\tOutput file" << std::endl
        << "-t, --threshold\tnumber\t\tSpecify the minimum percent the node has to have to be shown" << std::endl
        << "-n, --node\tpath-to-node\tOutput only the subtree of the node (path needs to be ';' separated)" << std::endl
        << "-l, --label\t\tOutput only the subtree of the node identified by the label" << std::endl
        << "-d, --depth\tinteger\t\tMax depth from the starting node" << std::endl
        << "-e, --export\t[DOT|GML|GRAPHML]\tThe output file format" << std::endl
        << "-c, --critical\t\t\tOutput critical path only" << std::endl;

    return ss.str();
}

void cmd_opt::parse_node(const char *text)
{
    NodePath path;
    path.literal = text;
    std::cout << "path: '" << text << "'" << std::endl;
    bool valid = false;
    while (true) {
        const char *p = std::strchr(text, ';');
        if (p == nullptr) break;
        path.node.push_back(std::string(text, p - text));
        valid = true;
        text = p + 1;
    }
    if (*text != '\0') {
        path.node.push_back(std::string(text));
        valid = true;
    }
    if (valid) {
        nodes.push_back(path);
    }
}

int cmd_opt::parse(int argc, char **argv)
{
    auto mode = CMD_OPT;
    for (auto i = 1; i < argc; i++) {
        std::string arg(argv[i]);
        switch (mode) {
        case CMD_OPT:
            if (arg == "-o" || arg == "--output") {
                mode = CMD_OUTPUT_ARG;
            }
            else if (arg == "-h" || arg == "--help") {
                return 1;
            }
            else if (arg == "-t" || arg == "--threshold") {
                mode = CMD_THRESHOLD_ARG;
            }
            else if (arg == "-n" || arg == "--node") {
                mode = CMD_NODE_ARG;
            }
            else if (arg == "-l" || arg == "--l") {
                mode = CMD_LABEL_ARG;
            }
            else if (arg == "-d" || arg == "--depth") {
                mode = CMD_DEPTH_ARG;
            }
            else if (arg == "-m" || arg == "--max-subnodes") {
                mode = CMD_MAX_SUBNODES_ARG;
            }
            else if (arg == "-e" || arg == "--export") {
                mode = CMD_EXPORT_ARG;
            }
            else if (arg == "-c" || arg == "--critical") {
                critical_only = true;
            }
            else {
                if (!ifile.empty()) {
                    return -3;
                }
                ifile = arg;
            }

            break;
        case CMD_OUTPUT_ARG:
            ofile = arg;
            mode = CMD_OPT;
            break;
        case CMD_THRESHOLD_ARG:
        {
            char *p = argv[i];
            threshold = std::strtod(argv[i], &p);
            if (p == argv[i]) {
                return -2;
            }
        }
        mode = CMD_OPT;
        break;
        case CMD_MAX_SUBNODES_ARG:
            try {
                max_subnodes = std::stoi(argv[i]);
            }
            catch (...) {
                return -10;
            }
            mode = CMD_OPT;
            break;
        case CMD_DEPTH_ARG:
            try {
                depth = std::stoi(argv[i]);
            }
            catch (...) {
                return -3;
            }
            mode = CMD_OPT;
            break;
        case CMD_EXPORT_ARG:
            if (!strcasecmp("DOT", argv[i])) {
                export_type = EXPORT_DOT;
            }
            else if (!strcasecmp("GML", argv[i])) {
                export_type = EXPORT_GML;
            }
            else if (!strcasecmp("GRAPHML", argv[i])) {
                export_type = EXPORT_GRAPHML;
            }
            else {
                return -4;
            }
            mode = CMD_OPT;
            break;
        case CMD_NODE_ARG:
            parse_node(argv[i]);
            mode = CMD_OPT;
            break;
        case CMD_LABEL_ARG:
        {
            std::string s((argv[i][1] == 'x' || argv[i][1] == 'X')? &argv[i][2] : argv[i]);
            auto i = std::stoll(s, NULL, 16);
            labels.push_back(i);
        }
            mode = CMD_OPT;
            break;
        default:
            return -1;
        }
    }

    return 0;
}

