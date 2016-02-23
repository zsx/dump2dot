#include "cmd_parse.h"
#include "dump.h"
#include "kind.h"

#include <iostream>
#include <fstream>
#include <iomanip>

enum MemoryDump::Parse_Result MemoryDump::parse(const char *buf, Node &node)
{
    const char comma = ',';
    while (*buf == ' ' || *buf == '\t') buf++;
    if (*buf == '#' || *buf == '\0') return PARSE_COMMENT; //ignore any line beginning with '#'
    const char *p = std::strchr(buf, comma);
    if (p == nullptr) return PARSE_FAIL;
    auto skip = buf[1] == 'x' ? 2 : 0;
    node.label = std::string("N") + std::string(buf + skip, p - buf - skip); /* skip '0x' */
    buf = p + 1;

    p = std::strchr(buf, comma);
    if (p == nullptr) return PARSE_FAIL;
    auto plabel = std::string(buf, p - buf);
    if (plabel != "(nil)") {
        plabel = std::string("N") + plabel.substr(plabel[1] == 'x' ? 2 : 0);
    }
    else {
        plabel = "NIL";
    }

    buf = p + 1;

    p = std::strchr(buf, comma);
    if (p == nullptr) return PARSE_FAIL;
    node.node_type = static_cast<enum Reb_Kind>(std::stoi(std::string(buf, p - buf)));
    buf = p + 1;

    p = std::strchr(buf, comma);
    if (p == nullptr) return PARSE_FAIL;
    node.subtree_size = node.size = std::stoi(std::string(buf, p - buf));
    buf = p + 1;

    p = std::strchr(buf, comma);
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
    if (opt.depth > 0 && level >= opt.depth) return true;
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
    if (opt.depth > 0 && level > opt.depth) return;
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

