#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>
#include <boost/property_map/property_map.hpp>
#include <catch.hpp>
#include <ckpttncpp/netlist.hpp>
#include <py2cpp/nx2bgl.hpp>
#include <utility> // for std::pair
#include <vector>

using graph_t =
    boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS>;
using node_t = typename boost::graph_traits<graph_t>::vertex_descriptor;
using edge_t = typename boost::graph_traits<graph_t>::edge_iterator;

#include <cassert>
#include <climits>
#include <fstream>
#include <vector>

using std::ifstream;
using std::ofstream;

// Read the IBM .netD/.net format. Precondition: Netlist is empty.
auto readNetD(const char *netDFileName) -> Netlist {
    ifstream netD(netDFileName);
    if (netD.fail()) {
        std::cerr << "Error: Can't open file " << netDFileName << ".\n";
        exit(1);
    }

    char t;
    size_t numPins, numNets, numModules, padOffset;

    netD >> t; // eat 1st 0
    netD >> numPins >> numNets >> numModules >> padOffset;

    using Edge = std::pair<int, int>;
    const int num_vertices = numModules + numNets;
    graph_t g(num_vertices);

    const size_t bufferSize = 100;
    char lineBuffer[bufferSize]; // Does it work for other compiler?
    netD.getline(lineBuffer, bufferSize);

    size_t w;
    size_t e = numModules - 1;
    char c;
    auto i = 0u;
    for (; i < numPins; ++i) {
        if (netD.eof()) {
            std::cerr << "Warning: Unexpected end of file.\n";
            break;
        }
        do {
            netD.get(c);
        } while (isspace(c) && c != EOF);
        if (c == '\n')
            continue;
        if (c == 'a') {
            netD >> w;
        } else if (c == 'p') {
            netD >> w;
            w += padOffset;
        }
        do {
            netD.get(c);
        } while (isspace(c) && c != EOF);
        if (c == 's') {
            ++e;
        }
    
        // edge_array[i] = Edge(w, e);
        boost::add_edge(w, e, g);
    
        do {
            netD.get(c);
        } while (isspace(c) && c != '\n' && c != EOF);
        // switch (c) {
        // case 'O': aPin.setDirection(Pin::OUTPUT); break;
        // case 'I': aPin.setDirection(Pin::INPUT); break;
        // case 'B': aPin.setDirection(Pin::BIDIR); break;
        // }
        if (c != '\n') {
            netD.getline(lineBuffer, bufferSize);
        }
    }

    e -= numModules - 1;
    if (e < numNets) {
        std::cerr << "Warning: number of nets is not " << numNets << ".\n";
        numNets = e;
    } else if (e > numNets) {
        std::cerr << "Error: number of nets is not " << numNets << ".\n";
        exit(1);
    }
    if (i < numPins) {
        std::cerr << "Error: number of pins is not " << numPins << ".\n";
        exit(1);
    }

    using IndexMap = typename boost::property_map<graph_t, boost::vertex_index_t>::type;
    IndexMap index = boost::get(boost::vertex_index, g);
    std::vector<node_t> module_list(numModules);
    std::vector<node_t> net_list(numNets);
    auto G = xn::grAdaptor<graph_t>(std::move(g));
    for (node_t v : G) {
        size_t i = index[v];
        if (i < numModules) {
            module_list[i] = v;
        }
        else {
            net_list[i - numModules] = v;
        }
    }
    auto H = Netlist(std::move(G), std::move(module_list), std::move(net_list));
    return H;
}

// Read the IBM .are format
// void readAre(const char *areFileName) {
//     ifstream are(areFileName);
//     if (are.fail()) {
//         std::cerr << " Could not open " << areFileName << std::endl;
//         return;
//     }

//     const size_t bufferSize = 100;
//     char lineBuffer[bufferSize];

//     char c;
//     size_t w;
//     size_t weight;
//     size_t totalWeight = 0;
//     // xxx size_t smallestWeight = UINT_MAX;

//     size_t lineno = 1;
//     for (size_t i = 0; i < _numModules; i++) {
//         if (are.eof())
//             break;
//         do
//             are.get(c);
//         while (isspace(c) && c != EOF);
//         if (c == '\n') {
//             lineno++;
//             continue;
//         }
//         if (c == 'a') {
//             are >> w;
//         } else if (c == 'p') {
//             are >> w;
//             w += _padOffset;
//         } else {
//             std::cerr << "Syntax error in line " << lineno << ":"
//                       << "expect keyword \"a\" or \"p\"" << std::endl;
//             exit(0);
//         }

//         Module &aModule = _moduleList[w];
//         do
//             are.get(c);
//         while (isspace(c) && c != EOF);
//         if (isdigit(c)) {
//             are.putback(c);
//             are >> weight;
//             aModule.setWeight(weight);
//             totalWeight += weight;
//         }
//         are.getline(lineBuffer, bufferSize);
//         lineno++;
//     }

//     // Update after the reading was sucessful
//     _totalWeight = totalWeight;
// }
