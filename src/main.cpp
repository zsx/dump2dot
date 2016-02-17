#include <iostream>
#include <set>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <fstream>

class cmd_opt {
	private:
		enum CMD_MODE {
			CMD_OPT,
			CMD_OUTPUT_ARG,
			CMD_THRESHOLD_ARG
		};
	public:
		std::string ifile;
		std::string ofile;
		double threshold;

        cmd_opt() : threshold(0) {}
		std::string help(const char* app);
		int parse(int argc, char **argv); 
};

std::string cmd_opt::help(const char* app)
{
	std::stringstream ss;
	ss << "Usage:" << std::endl
		<< app << " [options] input" << std::endl
		<< "-o, --output\tfile\tOutput file" << std::endl
		<< "-t, --threshold\tnumber\tSpecify the minimum percent the node has to have to be shown" << std::endl;

	return ss.str();
}

int cmd_opt::parse(int argc, char **argv)
{
    auto mode = CMD_OPT;
	for(auto i = 1; i < argc; i ++) {
		std::string arg(argv[i]);
		switch (mode) {
			case CMD_OPT:
				if (arg == "-o" || arg == "--output") {
					mode = CMD_OUTPUT_ARG;
				} else if (arg == "-t" || arg == "--threshold") {
					mode = CMD_THRESHOLD_ARG;
				} else {
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
			default:
				return -1;
		}
	}

    return 0;
}

struct Node {
	std::string label;
	std::set<std::string> parent_labels;
	int node_type;
	size_t size;
	double subtree_size;
	std::string name;
	std::vector<Node*> children;
    bool visited;

    Node(const std::string &label_ = "", const std::string &name_ = "") :
        label(label_), name(name_),
        node_type(0), size(0), subtree_size(0), visited(false) {}
};

struct MemoryDump {
	private:
		bool parse(const char *buf, Node &node);
		bool draw_tree(Node &node, std::ofstream &ofile, int level);
        void clear_visited(Node &);
		double total_size;
		double min_size;
	public:
		MemoryDump()
			:total_size(0),
			min_size(0)
		{}
		std::unordered_map<std::string, Node> nodes;
		std::vector<Node*> top_nodes;
		bool import(const std::string &path);
		double update_subtree_size(Node&, int);
		double update_subtree_size();
		bool write_output(const cmd_opt &opt);
        void clear_visited();
};

bool MemoryDump::parse(const char *buf, Node &node)
{
	const char comma = ',';
	const char *p = strchr(buf, comma);
	if (p == nullptr) return false;
	node.label = std::string("N") + std::string(buf + 2, p - buf - 2); /* skip '0x' */
	buf = p + 1;

	p = strchr(buf, comma);
	if (p == nullptr) return false;
    auto plabel = std::string(buf, p - buf);
    if (plabel != "(nil)") {
        plabel = std::string("N") + plabel.substr(2);
    }
    else {
        plabel = "NIL";
    }
    node.parent_labels.insert(plabel);
	buf = p + 1;

	p = strchr(buf, comma);
	if (p == nullptr) return false;
    node.node_type = std::stoi(std::string(buf, p - buf));
	buf = p + 1;

	p = strchr(buf, comma);
	if (p == nullptr) return false;
    node.subtree_size = node.size = std::stoi(std::string(buf, p - buf));
	buf = p + 1;

    node.name = buf;

	return true;
}

bool MemoryDump::import(const std::string &path)
{
	// Insert a top node
    Node nil("NIL", "NIL");
	nodes[nil.label] = nil;

	try {
		std::ifstream fi(path);
		while(fi.good()) {
			size_t s = 1024;
			char *buf = new char[s];
			fi.getline(buf, s);

			Node node;
			if (!parse(buf, node)) {
				std::cout << "Failed to parse line: " << buf << std::endl;
				continue;
			}
            auto iter = nodes.find(node.label);
			if (iter != nodes.end()) {
				std::cout << "Duplicate nodes: " << node.label << std::endl;
                iter->second.parent_labels.insert(*node.parent_labels.begin());
			} else {
				nodes[node.label] = node;
			}
		}
	} catch (...) {
		std::cout << "input error" << std::endl;
		return false;
	}

	for(auto &&pair : nodes) {
		auto &node = pair.second;
		bool top_level = true;
		for(const auto & parent_label : node.parent_labels) {
			auto parent = nodes.find(parent_label);
			if (parent != nodes.end()) {
				top_level = false;
				parent->second.children.push_back(&node);
			}
		}
		if (top_level) {
            top_nodes.push_back(&node);
		}
	}

	return true;
}

double MemoryDump::update_subtree_size(Node &node, int level = 0)
{
    if (node.visited) return 0;
    node.visited = true;
	for(auto c : node.children) {
		node.subtree_size += update_subtree_size(*c, level + 1) / c->parent_labels.size();
	}
	return node.subtree_size;
}

double MemoryDump::update_subtree_size()
{
	total_size = 0;
	for(auto node : top_nodes) {
		total_size += update_subtree_size(*node);
	}
    clear_visited();
	return total_size;
}

void MemoryDump::clear_visited(Node &node)
{
    if (!node.visited) return;
    node.visited = false;
    for (auto c : node.children) {
        clear_visited(*c);
    }
}

void MemoryDump::clear_visited()
{
    for (const auto &node : top_nodes) {
        clear_visited(*node);
    }
}

bool MemoryDump::draw_tree(Node &node, std::ofstream &ofile, int level = 0)
{
    if (node.visited) return true;
    node.visited = true;
	for(const auto c : node.children) {
		if (c->subtree_size >= min_size) {
			ofile << node.label << " -> " << c->label << std::endl;
			draw_tree(*c, ofile, level + 1);
		}
	}
    return true;
}

bool MemoryDump::write_output(const cmd_opt &opt)
{
	try {
		std::ofstream ofile(opt.ofile);
        min_size = total_size * opt.threshold;
        ofile << std::string("strict digraph dump {\n");
		for(const auto &pair : nodes) {
			const auto &node = pair.second;
            if (node.subtree_size < min_size) continue;
            ofile << node.label << "[label=\"" << (node.name == "(null)" ? node.label : node.name)
                << "," << static_cast<size_t>(node.subtree_size) << "(" << static_cast<size_t>(node.size) << ")\"];\n";
		}
		for(const auto &node : top_nodes) {
			draw_tree(*node, ofile);
		}
		ofile << std::string("}") << std::endl;
        clear_visited();
	} catch (...) {
		std::cout << "Unexpected error hanppend while writing output" << std::endl;
	}

    return true;
}

int main(int argc, char **argv)
{
	cmd_opt opt;
	try {
		if (opt.parse(argc, argv) < 0) {
			std::cout << "Wrong command line" << std::endl;
			std::cout << opt.help(argv[0]);
//			return EXIT_FAILURE;
		}
	} catch (...) {
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
	} catch (...) {
		std::cout << "Unexpected error hanppend" << std::endl;
	}

    return EXIT_SUCCESS;
}
