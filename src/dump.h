#ifndef D2D_DUMP_H
#define D2D_DUMP_H

#include "kind.h"
#include <vector>
#include <string>
#include <set>
#include <unordered_map>

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

#endif //D2D_DUMP_H
