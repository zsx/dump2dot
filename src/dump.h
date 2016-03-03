#ifndef D2D_DUMP_H
#define D2D_DUMP_H

#include <vector>
#include <string>
#include <set>
#include <unordered_map>

#include "kind.h"
#include "export.h"

enum EdgePriority {
    EDGE_PRIORITY_MIN = 0,
    EDGE_PRIORITY_CHUNK_VALUE,
    EDGE_PRIORITY_BOUND_TO,
    EDGE_PRIORITY_PARENT, /* gob parent */
    EDGE_PRIORITY_DEFAULT,
    EDGE_PRIORITY_MAX
};

class StringBin {
private:
    int id;
public:
    static std::vector<std::string> array;
    static std::unordered_map<std::string, std::pair<int, int>> set;
    const std::string str() const
    {
        if (id < 0 || id >= array.size()) return "";
        return array[id];
    }

    void str(const std::string &s)
    {
        auto iter = set.find(s);
        if (iter == set.end()) {
            array.push_back(s);
            set[s].first = id = array.size() - 1;
            set[s].second = 0;
        }
        else {
            id = iter->second.first;
            iter->second.second++;
        }
    }

    void erase()
    {
        if (id < 0 || id >= array.size()) return;
        auto iter = set.find(array[id]);
        iter->second.second--;
        id = -1;
    }

    bool is_valid() const
    {
        return id >= 0 && id < array.size();
    }

    StringBin(const std::string &s)
    {
        str(s);
    }

    StringBin(const StringBin &s)
    {
        id = s.id;
    }
};

struct ParentNode {
    uintptr_t label;
    StringBin edge;
    enum EdgePriority priority;
    ParentNode(uintptr_t label_, const std::string &edge_) :
        label(label_),
        edge(edge_),
        priority(EDGE_PRIORITY_DEFAULT)
    {
        if (edge.str() == "<bound-to>") {
            priority = EDGE_PRIORITY_BOUND_TO;
        }
        else if (edge.str() == "<parent>") {
            priority = EDGE_PRIORITY_PARENT;
        }
        else if (edge.str() == "<keeps>") {
            priority = EDGE_PRIORITY_CHUNK_VALUE;
        }
    }
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
    StringBin edge;
    ChildNode (Node *node_, const std::string &edge_)
        : node (node_),
        edge(edge_) {}
    ChildNode(Node *node_, const StringBin &edge_)
        : node(node_),
        edge(edge_) {}
};

struct Node {
    uintptr_t label;
    std::set<ParentNode, ParentNodeComp> parents;
    enum Reb_Kind node_type;
    size_t size;
    double subtree_size;
    StringBin name;
    std::vector<ChildNode> children;
    int visited;
    bool critical;
    int subtree_size_division; /* how much the subtree_size contributes its parents' subtree_size */

    Node(uintptr_t label_ = 0, const std::string &name_ = "") :
        label(label_), name(name_),
        node_type(REB_TRASH),
        size(0),
        subtree_size(0),
        visited(-1),
        critical(false),
        subtree_size_division(0) {}
};

class MemoryDump {
private:
    enum Parse_Result {
        PARSE_OK,
        PARSE_FAIL,
        PARSE_COMMENT
    };
    enum Parse_Result parse(const char *buf, Node &node);
    bool draw_tree(Node &node, std::ofstream &ofile, const cmd_opt &opt, std::set<uintptr_t> &declared_nodes, int level = 0);
    void clear_visited(Node &);
    void clear_visited();
    void set_critical(const std::vector<ChildNode> &nodes, int level = 0);
    void pre_update_subtree_size(Node &node, std::set<uintptr_t> &path);
    Node *find_node(std::vector<std::string> path) const;
    void write_node(const Node &, std::ofstream&, const cmd_opt &);

    double total_size;
    double min_size;
    std::unordered_map<uintptr_t, Node> nodes;
    std::vector<ChildNode> top_nodes;
    Exporter *exporter;
    enum ExportType export_type;
public:
    MemoryDump()
        :total_size(0),
        min_size(0),
        exporter(nullptr)
    {}

    virtual ~MemoryDump()
    {
        if (exporter != nullptr) {
            delete exporter;
        }
    }

    bool import(const std::string &path);
    double update_subtree_size(Node &node, std::set<uintptr_t> &path);
    double update_subtree_size();
    bool write_output(const cmd_opt &opt);
    void reset();
};

#endif //D2D_DUMP_H
