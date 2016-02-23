#ifndef D2D_CMD_PARSE_H
#define D2D_CMD_PARSE_H

#include <vector>
#include <string>

class cmd_opt {
private:
    enum CMD_MODE {
        CMD_OPT,
        CMD_OUTPUT_ARG,
        CMD_THRESHOLD_ARG,
        CMD_DEPTH_ARG,
        CMD_NODE_ARG
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
    bool critical_only;
    std::vector<NodePath> nodes;

    cmd_opt() : threshold(0), critical_only(false), depth(-1) {}
    std::string help(const char* app);
    int parse(int argc, char **argv);
};

#endif //D2D_CMD_PARSE_H
