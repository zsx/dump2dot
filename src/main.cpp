#include <iostream>
#include <set>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <fstream>
#include <iomanip>

#include "kind.h"

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
                const char *p = strchr(buf, ';');
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

struct ParentNode {
    std::string label;
    std::string edge;
    ParentNode(std::string label_, std::string edge_) :
        label(label_),
        edge(edge_)
    {}
};

class ParentNodeComp {
public:
    bool operator () (const ParentNode &a, const ParentNode &b) const {
        return a.label < b.label;
    }
};

struct Node;

struct ChildNode {
    Node* node;
    std::string edge;
};

struct Node {
    std::string label;
    std::set<ParentNode, ParentNodeComp> parents;
    enum Reb_Kind node_type;
    size_t size;
    double subtree_size;
    std::string name;
    std::vector<ChildNode> children;
    int visited;
    bool critical;

    Node(const std::string &label_ = "", const std::string &name_ = "") :
        label(label_), name(name_),
        node_type(REB_TRASH),
        size(0),
        subtree_size(0),
        visited(-1),
        critical(false) {}
};

struct MemoryDump {
private:
    enum Parse_Result {
        PARSE_OK,
        PARSE_FAIL,
        PARSE_COMMENT
    };
    enum Parse_Result parse(const char *buf, Node &node);
    bool draw_tree(Node &node, std::ofstream &ofile, const cmd_opt &opt, int level = 0);
    void clear_visited(Node &);
    void clear_visited();
    void set_critical(const std::vector<ChildNode> &nodes, int level = 0);
    Node *find_node(std::vector<std::string> path) const;
    void write_node(const Node &, std::ofstream&, const cmd_opt &);
    void declare_nodes(Node &node, std::ofstream &ofile, const cmd_opt &opt, int level = 0);
    double total_size;
    double min_size;
public:
    MemoryDump()
        :total_size(0),
        min_size(0)
    {}
    std::unordered_map<std::string, Node> nodes;
    std::vector<ChildNode> top_nodes;
    bool import(const std::string &path);
    double update_subtree_size(Node&, int level = 0);
    double update_subtree_size();
    bool write_output(const cmd_opt &opt);
};

enum MemoryDump::Parse_Result MemoryDump::parse(const char *buf, Node &node)
{
    const char comma = ',';
    while (*buf == ' ' || *buf == '\t') buf++;
    if (*buf == '#' || *buf == '\0') return PARSE_COMMENT; //ignore any line beginning with '#'
    const char *p = strchr(buf, comma);
    if (p == nullptr) return PARSE_FAIL;
    auto skip = buf[1] == 'x' ? 2 : 0;
    node.label = std::string("N") + std::string(buf + skip, p - buf - skip); /* skip '0x' */
    buf = p + 1;

    p = strchr(buf, comma);
    if (p == nullptr) return PARSE_FAIL;
    auto plabel = std::string(buf, p - buf);
    if (plabel != "(nil)") {
        plabel = std::string("N") + plabel.substr(plabel[1] == 'x' ? 2 : 0);
    }
    else {
        plabel = "NIL";
    }

    buf = p + 1;

    p = strchr(buf, comma);
    if (p == nullptr) return PARSE_FAIL;
    node.node_type = static_cast<enum Reb_Kind>(std::stoi(std::string(buf, p - buf)));
    buf = p + 1;

    p = strchr(buf, comma);
    if (p == nullptr) return PARSE_FAIL;
    node.subtree_size = node.size = std::stoi(std::string(buf, p - buf));
    buf = p + 1;

    p = strchr(buf, comma);
    if (p == nullptr) return PARSE_FAIL;
    auto edge = std::string(buf, p - buf);
    if (edge == "(null)") {
        edge.erase();
    }
    buf = p + 1;

    node.parents.insert(ParentNode(plabel, edge));

    node.name = buf;
    if (node.name == "(null)") {
        node.name = node.label;
    }

    return PARSE_OK;
}

bool MemoryDump::import(const std::string &path)
{
    // Insert a top node
    Node nil("NIL", "NIL");
    nodes[nil.label] = nil;

    try {
        std::ifstream fi(path);
        size_t i = 0;
        while (fi.good()) {
            size_t s = 1024;
            char *buf = new char[s];
            fi.getline(buf, s);

            ++i;
            Node node;
            auto parse_result = parse(buf, node);
            if (parse_result == PARSE_FAIL) {
                std::cout << "Failed to parse line " << i << ": " << buf << std::endl;
                continue;
            }
            else if (parse_result == PARSE_COMMENT) {
                continue;
            }
            auto iter = nodes.find(node.label);
            if (iter != nodes.end()) {
                //std::cout << "Duplicate nodes: " << node.label << std::endl;
                for (const auto & p : node.parents) {
                    iter->second.parents.insert(p);
                }
            }
            else {
                nodes[node.label] = node;
            }
        }
    }
    catch (...) {
        std::cout << "input error" << std::endl;
        return false;
    }

    for (auto &&pair : nodes) {
        auto &node = pair.second;
        bool top_level = true;
        for (const auto & p : node.parents) {
            auto parent = nodes.find(p.label);
            if (parent != nodes.end()) {
                top_level = false;
                ChildNode c = { &node, p.edge };
                parent->second.children.push_back(c);
            }
        }
        if (top_level) {
            ChildNode c = { &pair.second, "" };
            top_nodes.push_back(c);
        }
    }

    return true;
}

Node *MemoryDump::find_node(std::vector<std::string> path) const
{
    auto *v = &top_nodes;
    Node *ret;
    for (const auto & name : path) {
        bool found = false;
        for (const auto & node : *v) {
            if (node.node->name == name) {
                found = true;
                ret = node.node;
                break;
            }
        }
        if (!found) return nullptr;
        v = &ret->children;
    }

    return ret;
}

double MemoryDump::update_subtree_size(Node &node, int level)
{
    if (node.visited >= 0) return 0;
    node.visited = level;
    for (auto c : node.children) {
        node.subtree_size += update_subtree_size(*c.node, level + 1);
    }
    return node.subtree_size;
}

double MemoryDump::update_subtree_size()
{
    total_size = 0;
    for (auto node : top_nodes) {
        total_size += update_subtree_size(*node.node);
    }
    clear_visited();

    set_critical(top_nodes);
    clear_visited();
    return total_size;
}

void MemoryDump::clear_visited(Node &node)
{
    if (node.visited < 0) return;
    node.visited = -1;
    for (auto c : node.children) {
        clear_visited(*c.node);
    }
}

void MemoryDump::clear_visited()
{
    for (const auto &node : top_nodes) {
        clear_visited(*node.node);
    }
}

void MemoryDump::set_critical(const std::vector<ChildNode> &nodes, int level)
{
    double m = -1;
    for (auto node : nodes) {
        if (node.node->subtree_size > m) {
            m = node.node->subtree_size;
        }
    }
    for (auto node : nodes) {
        if (node.node->subtree_size >= 0.5 * m) {
            if (node.node->visited) continue;
            node.node->visited = level;
            node.node->critical = true;
            set_critical(node.node->children, level + 1);
        }
    }
}

bool MemoryDump::draw_tree(Node &node, std::ofstream &ofile, const cmd_opt &opt, int level)
{
    if (node.visited >= 0 && node.visited <= level) return true;
    if (opt.critical_only && !node.critical) return true;
    if (level >= opt.depth) return true;
    node.visited = level;
    for (const auto c : node.children) {
        if (c.node->subtree_size >= min_size
            && (!opt.critical_only || c.node->critical)) {
            ofile << node.label << " -> " << c.node->label;
            if (!c.edge.empty()) {
                ofile << "[label = \"" << c.edge << "\"]";
            }
            ofile << ";\n";
            draw_tree(*c.node, ofile, opt, level + 1);
        }
    }
    return true;
}

void MemoryDump::write_node(const Node &node, std::ofstream &ofile, const cmd_opt &opt)
{
    if (node.subtree_size < min_size) return;
    if (opt.critical_only && !node.critical) return;
    ofile << node.label << "[label=\"" << node.name
        << "\\n" << kind2str[node.node_type]
        << "\\n" << static_cast<size_t>(node.subtree_size) << "(" << static_cast<size_t>(node.size) << ")"
        << "\\n" << std::fixed << std::setprecision(2) << (node.subtree_size * 100 / total_size) << "%"
        << "(" << (node.size * 100 / total_size) << "%"
        << ")\", "
        << "shape=box";
    if (node.critical && !opt.critical_only) {
        ofile << ", style=filled, fillcolor=yellow";
    }
    ofile << "];\n";
}

void MemoryDump::declare_nodes(Node &node, std::ofstream &ofile, const cmd_opt &opt, int level)
{
    if (node.visited >= 0 && node.visited <= level) return;
    if (level > opt.depth) return;
    write_node(node, ofile, opt);
    node.visited = level;
    for (auto c : node.children) {
        declare_nodes(*c.node, ofile, opt, level + 1);
    }
}

bool MemoryDump::write_output(const cmd_opt &opt)
{
    try {
        std::ofstream ofile(opt.ofile);
        min_size = total_size * opt.threshold;
        ofile << std::string("strict digraph dump {\n");
        std::vector<Node*> selected_nodes;
        if (opt.nodes.empty()) {
            for (const auto &c : top_nodes) {
                selected_nodes.push_back(c.node);
            }
        }
        else {
            /* find the node pointed by opt.node */
            for (const auto &path : opt.nodes) {
                auto node = find_node(path.node);
                if (node == nullptr) {
                    std::cout << "No node found for path " << path.literal << std::endl;
                    continue;
                }
                selected_nodes.push_back(node);
            }
        }

        for (auto & node : selected_nodes) {
            declare_nodes(*node, ofile, opt);
        }
        for (auto & node : selected_nodes) {
            clear_visited(*node);
        }

        for (auto & node : selected_nodes) {
            draw_tree(*node, ofile, opt);
        }
        for (auto & node : selected_nodes) {
            clear_visited(*node);
        }

        ofile << std::string("}") << std::endl;

    }
    catch (...) {
        std::cout << "Unexpected error hanppend while writing output" << std::endl;
    }

    return true;
}

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
