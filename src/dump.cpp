#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cstring>

#include "cmd_parse.h"
#include "dump.h"
#include "kind.h"
#include "export.h"
#include "export_graphml.h"
#include "export_dot.h"
#include "export_gml.h"

std::vector<std::string> StringBin::array;
std::unordered_map<std::string, std::pair<int, int>> StringBin::set;

enum MemoryDump::Parse_Result MemoryDump::parse(const char *buf, Node &node)
{
    const char comma = ',';
    while (*buf == ' ' || *buf == '\t') buf++;
    if (*buf == '#' || *buf == '\0') return PARSE_COMMENT; //ignore any line beginning with '#'
    const char *p = std::strchr(buf, comma);
    if (p == nullptr) return PARSE_FAIL;
    auto skip = buf[1] == 'x' ? 2 : 0;
    auto s = std::string(buf + skip, p - buf - skip);
    size_t pos = 0;
    node.label = std::stoll(s, &pos, 16); /* skip '0x' */
    buf = p + 1;

    p = std::strchr(buf, comma);
    if (p == nullptr) return PARSE_FAIL;
    s = std::string(buf, p - buf);
    uintptr_t plabel;
    if (s != "(nil)") {
        plabel = std::stoll(s.substr(s[1] == 'x' ? 2 : 0), &pos, 16); /* skip '0x' */
    }
    else {
        plabel = 0;
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

    if (!std::strcmp("(null)", buf)) {
        node.name.erase();
    }
    else {
        node.name.str(buf);
    }

    return PARSE_OK;
}

void MemoryDump::reset()
{
    total_size = min_size = 0;
    nodes.clear();
    top_nodes.clear();
}

bool MemoryDump::import(const std::string &path)
{
    reset();

    // Insert a top node
    Node nil(0, "NIL");
    nodes[nil.label] = nil;

    std::cout << "sizeof(node): " << sizeof(Node) << std::endl;

    try {
        std::ifstream fi(path);
        size_t i = 0;
        size_t s = 1024;
        char *buf = new char[s];
        while (fi.good()) {
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
            if (i > 1000000) {
                if (i % 1000000 == 0) {
                    std::cout << "Imported " << i << " lines" << std::endl;
                }
            }
            else if (i > 100000) {
                if (i % 100000 == 0) {
                    std::cout << "Imported " << i << " lines" << std::endl;
                }
            }
            else if (i > 10000) {
                if (i % 10000 == 0) {
                    std::cout << "Imported " << i << " lines" << std::endl;
                }
            }
            else if (i > 1000) {
                if (i % 1000 == 0) {
                    std::cout << "Imported " << i << " lines" << std::endl;
                }
            }
            else {
                if (i % 100 == 0) {
                    std::cout << "Imported " << i << " lines" << std::endl;
                }
            }
        }
        delete buf;
        std::cout << i << " nodes imported\n";
    }
    catch (...) {
        std::cout << "input error" << std::endl;
        return false;
    }

    for (auto &&pair : nodes) {
        auto &node = pair.second;
        bool top_level = true;

        /* find the top edge priority */
        enum EdgePriority priority = EDGE_PRIORITY_MIN;
        for (const auto & p : node.parents) {
            if (p.priority > priority) {
                priority = p.priority;
            }
        }

        for (auto & p : node.parents) {
            if (p.priority == priority) { // only take the top priority one
                auto parent = nodes.find(p.label);
                if (parent != nodes.end()) {
                    top_level = false;
                    ChildNode c = { &node, p.edge };
                    parent->second.children.push_back(c);
                }
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
            if (node.node->name.str() == name) {
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

void MemoryDump::pre_update_subtree_size(Node &node, std::set<uintptr_t> &path)
{
    if (path.find(node.label) != path.end()) return;
    node.subtree_size_division++;
    if (node.visited >= 0) return;
    node.visited = 1;
    auto pair = path.insert(node.label);
    for (auto c : node.children) {
        pre_update_subtree_size(*c.node, path);
    }
    if (pair.second) {
        path.erase(pair.first);
    }
}

double MemoryDump::update_subtree_size(Node &node, std::set<uintptr_t> &path)
{
    if (path.find(node.label) != path.end()) return 0;
    if (node.visited >= 0) return node.subtree_size / node.subtree_size_division;
    node.visited = 1;
    auto pair = path.insert(node.label);
    for (auto c : node.children) {
        node.subtree_size += update_subtree_size(*c.node, path);
    }
    if (pair.second) {
        path.erase(pair.first);
    }
    return node.subtree_size / node.subtree_size_division;
}

double MemoryDump::update_subtree_size()
{
    total_size = 0;
    for (auto node : top_nodes) {
        std::set<uintptr_t> path;
        pre_update_subtree_size(*node.node, path);
        clear_visited(*node.node);
    }

    for (auto node : top_nodes) {
        std::set<uintptr_t> path;
        total_size += update_subtree_size(*node.node, path);
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
            if (node.node->visited >= 0) continue;
            node.node->visited = level;
            node.node->critical = true;
            set_critical(node.node->children, level + 1);
        }
    }
}

bool MemoryDump::draw_tree(Node &node, std::ofstream &ofile, const cmd_opt &opt, std::set<uintptr_t> &declared_nodes, int level)
{
    if (node.visited >= 0 && node.visited <= level) return true;
    if (opt.critical_only && !node.critical) return true;
    if (opt.depth > 0 && level >= opt.depth) return true;
    node.visited = level;
    if (declared_nodes.find(node.label) == declared_nodes.end()) {
        write_node(node, ofile, opt);
        declared_nodes.insert(node.label);
    }
    for (const auto c : node.children) {
        if (c.node->subtree_size >= min_size
            && (!opt.critical_only || c.node->critical)) {
            if (declared_nodes.find(c.node->label) == declared_nodes.end()) {
                write_node(*c.node, ofile, opt);
                declared_nodes.insert(c.node->label);
            }
            exporter->write_edge(node, *c.node, ofile, c.edge.str());
            draw_tree(*c.node, ofile, opt, declared_nodes, level + 1);
        }
    }
    return true;
}

void MemoryDump::write_node(const Node &node, std::ofstream &ofile, const cmd_opt &opt)
{
    exporter->write_node(node, ofile, opt);
}

bool MemoryDump::write_output(const cmd_opt &opt)
{
    try {
        std::ofstream ofile(opt.ofile, std::ofstream::trunc);
        min_size = total_size * opt.threshold;
        if (exporter == nullptr
            || export_type != opt.export_type) {
            delete exporter;
            export_type = opt.export_type;
            switch (export_type) {
            case EXPORT_DOT:
                exporter = new ExporterDot(total_size);
                break;
            case EXPORT_GML:
                exporter = new ExporterGML(total_size);
                break;
            case EXPORT_GRAPHML:
                exporter = new ExporterGraphML();
                break;
            default:
                exporter = new ExporterDot(total_size);
                export_type = EXPORT_DOT;
            }
        }
        else {
            switch (export_type) {
            case EXPORT_DOT:
            case EXPORT_GML:
                exporter->set_total_size(total_size);
                break;
            default:
                break;
            }
        }
        exporter->write_preamble(ofile);
        std::vector<Node*> selected_nodes;
        if (opt.nodes.empty() && opt.labels.empty()) {
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

            for (const auto &label : opt.labels) {
                auto node = nodes.find(label);
                if (node != nodes.end()) {
                    std::cout << "Found node by label " << std::hex << label << std::dec << std::endl;
                    selected_nodes.push_back(&node->second);
                }
                else {
                    std::cout << "Label " << std::hex << label << std::dec << " was not found\n";
                }
            }
        }
        std::set<uintptr_t> declared_nodes;
        for (auto & node : selected_nodes) {
            draw_tree(*node, ofile, opt, declared_nodes);
        }
        for (auto & node : selected_nodes) {
            clear_visited(*node);
        }

        exporter->write_appendix(ofile);

    }
    catch (...) {
        std::cout << "Unexpected error hanppend while writing output" << std::endl;
    }

    return true;
}

