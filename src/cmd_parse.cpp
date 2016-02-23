#include <sstream>

#include "cmd_parse.h"
#include <cstring>

std::string cmd_opt::help(const char* app)
{
    std::stringstream ss;
    ss << "Usage:" << std::endl
        << app << " [options] input" << std::endl
        << "-h, --help\t\t\tThis help" << std::endl
        << "-o, --output\tfile\t\tOutput file" << std::endl
        << "-t, --threshold\tnumber\t\tSpecify the minimum percent the node has to have to be shown" << std::endl
        << "-n, --node\tpath-to-node\tOutput only the subtree of the node (path needs to be ';' separated)" << std::endl
        << "-d, --depth\tintEger\t\tMax depth from the starting node" << std::endl
        << "-c, --critical\t\t\tOutput critical path only" << std::endl;

    return ss.str();
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
            else if (arg == "-d" || arg == "--depth") {
                mode = CMD_DEPTH_ARG;
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
        case CMD_DEPTH_ARG:
            try {
                depth = std::stoi(argv[i]);
            }
            catch (...) {
                return -3;
            }
            mode = CMD_OPT;
            break;
        case CMD_NODE_ARG:
        {
            const char *buf = argv[i];
            NodePath path;
            path.literal = buf;
            while (true) {
                const char *p = std::strchr(buf, ';');
                if (p == nullptr) break;
                path.node.push_back(std::string(buf, p - buf));
                buf = p + 1;
            }
            if (*buf != '\0') {
                path.node.push_back(std::string(buf));
            }
            nodes.push_back(path);
        }
        mode = CMD_OPT;
        break;
        default:
            return -1;
        }
    }

    return 0;
}

