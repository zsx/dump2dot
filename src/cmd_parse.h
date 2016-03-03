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
    bool critical_only;
    enum ExportType export_type;
    std::vector<NodePath> nodes;
    std::vector<uintptr_t> labels;

    cmd_opt() : threshold(0), critical_only(false), depth(-1), export_type(EXPORT_DOT) {}
    std::string help(const char* app);
    void parse_node(const char *text);
    int parse(int argc, char **argv);
};

#endif //D2D_CMD_PARSE_H
