
.. _program_listing_file__media_lubuntu_USBDISK_github_ckpttncpp_lib_include_xnetwork_classes_graph.hpp:

Program Listing for File graph.hpp
==================================

|exhale_lsh| :ref:`Return to documentation for file <file__media_lubuntu_USBDISK_github_ckpttncpp_lib_include_xnetwork_classes_graph.hpp>` (``/media/lubuntu/USBDISK/github/ckpttncpp/lib/include/xnetwork/classes/graph.hpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #ifndef _HOME_UBUNTU_GITHUB_XNETWORK_CLASS_GRAPH_HPP
   #define _HOME_UBUNTU_GITHUB_XNETWORK_CLASS_GRAPH_HPP 1
   
   // #include <boost/any.hpp>
   #include <any>
   #include <cassert>
   #include <py2cpp/py2cpp.hpp>
   #include <type_traits>
   #include <vector>
   #include <xnetwork/classes/coreviews.hpp> // import AtlasView, AdjacencyView
   #include <xnetwork/classes/reportviews.hpp> // import NodeView, EdgeView, DegreeView
   
   namespace xn
   {
   
   struct object : py::dict<const char*, std::any>
   {
   };
   
   template <typename nodeview_t, typename nodemap_t,
       typename adjlist_inner_dict_factory =
           py::set<typename nodeview_t::value_type>>
   class Graph : public object
   {
     public:
       using Node = typename nodeview_t::value_type; // luk
       using dict = py::dict<const char*, std::any>;
       using graph_attr_dict_factory = dict;
       // using edge_attr_dict_factory = dict;
       // using node_attr_dict_factory = dict;
       // using node_dict_factory = py::dict<Node, node_attr_dict_factory>;
       // using adjlist_inner_dict_factory = py::dict<Node,
       // edge_attr_dict_factory>;
       // using adjlist_inner_dict_factory = py::set<Node>;
       using adjlist_outer_dict_factory = std::vector<adjlist_inner_dict_factory>;
       using key_type = typename adjlist_inner_dict_factory::key_type;
       using value_type = typename adjlist_inner_dict_factory::value_type;
   
     public:
       // std::vector<Node > _Nodes{};
       nodeview_t _node;
       nodemap_t _node_map;
       graph_attr_dict_factory graph {}; // dictionary for graph attributes
       // node_dict_factory _node{};  // empty node attribute dict
       adjlist_outer_dict_factory _adj; // empty adjacency dict
   
       // auto __getstate__( ) {
       //     attr = this->__dict__.copy();
       //     // remove lazy property attributes
       //     if ("nodes" : attr) {
       //         del attr["nodes"];
       //     }
       //     if ("edges" : attr) {
       //         del attr["edges"];
       //     }
       //     if ("degree" : attr) {
       //         del attr["degree"];
       //     }
       //     return attr;
       // }
   
       Graph(const nodeview_t& Nodes, const nodemap_t& node_map)
           : _node {Nodes}
           , _node_map {node_map}
           , _adj(Nodes.size())
       {
       }
   
       explicit Graph(int num_nodes)
           : _node {py::range<int>(num_nodes)}
           , _node_map {py::range<int>(num_nodes)}
           , _adj(num_nodes)
       {
       }
   
   
       auto adj() const
       {
           return AdjacencyView(this->_adj);
       }
   
       auto get_name()
       {
           if (!this->graph.contains("name"))
               return "";
           return std::any_cast<const char*>(this->graph["name"]);
       }
   
       // @name.setter
       auto set_name(const char* s)
       {
           this->graph["name"] = std::any(s);
       }
   
       auto begin() const
       {
           return std::begin(this->_node);
       }
   
       auto end() const
       {
           return std::end(this->_node);
       }
   
       bool contains(const Node& n)
       {
           return this->_node.contains(n);
       }
   
       auto operator[](const Node& n) const
       {
           return this->adj()[this->_node_map[n]];
       }
   
       auto nodes()
       {
           auto nodes = NodeView(*this);
           // Lazy View creation: overload the (class) property on the instance
           // Then future G.nodes use the existing View
           // setattr doesn"t work because attribute already exists
           this->operator[]("nodes") = std::any(nodes);
           return nodes;
       }
   
       auto number_of_nodes() const
       {
           return this->_node.size();
       }
   
       auto order()
       {
           return this->_node.size();
       }
   
       auto has_node(const Node& n)
       {
           return this->_node.contains(n);
       }
   
       auto add_edge(const Node& u, const Node& v)
       {
           // auto [u, v] = u_of_edge, v_of_edge;
           // add nodes
           assert(this->_node.contains(u));
           assert(this->_node.contains(v));
           // add the edge
           // datadict = this->_adj[u].get(v, this->edge_attr_dict_factory());
           // datadict.update(attr);
           if constexpr (std::is_same_v<key_type, value_type>)
           {
               // set
               this->_adj[this->_node_map[u]].insert(v);
               this->_adj[this->_node_map[v]].insert(u);
           }
           else
           {
               using T = typename adjlist_inner_dict_factory::mapped_type;
               auto data = this->_adj[this->_node_map[u]].get(v, T {});
               this->_adj[this->_node_map[u]][v] = data;
               this->_adj[this->_node_map[v]][u] = data; // ???
           }
       }
   
       auto has_edge(const Node& u, const Node& v) -> bool
       {
           return this->_adj[this->_node_map[u]].contains(v);
       }
   
       auto degree(const Node& n)
       {
           return this->_adj[this->_node_map[n]].size();
       }
   
       // /// @property
       // auto edges( ) {
       //     /** An EdgeView of the Graph as G.edges || G.edges().
   
       //     edges( nbunch=None, data=false, default=None);
   
       //     The EdgeView provides set-like operations on the edge-tuples
       //     as well as edge attribute lookup. When called, it also provides
       //     an EdgeDataView object which allows control of access to edge
       //     attributes (but does not provide set-like operations).
       //     Hence, `G.edges[u, v]["color"]` provides the value of the color
       //     attribute for edge `(u, v)` while
       //     `for (auto u, v, c] : G.edges.data("color", default="red") {`
       //     iterates through all the edges yielding the color attribute
       //     with default `"red"` if (no color attribute exists.
   
       //     Parameters
       //     ----------
       //     nbunch : single node, container, || all nodes (default= all nodes);
       //         The view will only report edges incident to these nodes.
       //     data : string || bool, optional (default=false);
       //         The edge attribute returned : 3-tuple (u, v, ddict[data]).
       //         If true, return edge attribute dict : 3-tuple (u, v, ddict).
       //         If false, return 2-tuple (u, v).
       //     default : value, optional (default=None);
       //         Value used for edges that don"t have the requested attribute.
       //         Only relevant if (data is not true || false.
   
       //     Returns
       //     -------
       //     edges : EdgeView
       //         A view of edge attributes, usually it iterates over (u, v);
       //         || (u, v, d) tuples of edges, but can also be used for
       //         attribute lookup as `edges[u, v]["foo"]`.
   
       //     Notes
       //     -----
       //     Nodes : nbunch that are not : the graph will be (quietly) ignored.
       //     For directed graphs this returns the out-edges.
   
       //     Examples
       //     --------
       //     >>> G = xn::path_graph(3)   // or MultiGraph, etc
       //     >>> G.add_edge(2, 3, weight=5);
       //     >>> [e for e : G.edges];
       //     [(0, 1), (1, 2), (2, 3)];
       //     >>> G.edges.data();  // default data is {} (empty dict);
       //     EdgeDataView([(0, 1, {}), (1, 2, {}), (2, 3, {"weight": 5})]);
       //     >>> G.edges.data("weight", default=1);
       //     EdgeDataView([(0, 1, 1), (1, 2, 1), (2, 3, 5)]);
       //     >>> G.edges([0, 3]);  // only edges incident to these nodes
       //     EdgeDataView([(0, 1), (3, 2)]);
       //     >>> G.edges(0);  // only edges incident to a single node (use
       //     G.adj[0]?); EdgeDataView([(0, 1)]);
       //      */
       //     auto edges = EdgeView(*this);
       //     this->operator[]("edges") = std::any(edges);
       //     return edges;
       // }
   
       // /// @property
       // auto degree( ) {
       //     /** A DegreeView for the Graph as G.degree || G.degree().
   
       //     The node degree is the number of edges adjacent to the node.
       //     The weighted node degree is the sum of the edge weights for
       //     edges incident to that node.
   
       //     This object provides an iterator for (auto node, degree) as well as
       //     lookup for the degree for a single node.
   
       //     Parameters
       //     ----------
       //     nbunch : single node, container, || all nodes (default= all nodes);
       //         The view will only report edges incident to these nodes.
   
       //     weight : string || None, optional (default=None);
       //        The name of an edge attribute that holds the numerical value used
       //        as a weight.  If None, then each edge has weight 1.
       //        The degree is the sum of the edge weights adjacent to the node.
   
       //     Returns
       //     -------
       //     If a single node is requested
       //     deg : int
       //         Degree of the node
   
       //     OR if (multiple nodes are requested
       //     nd_view : A DegreeView object capable of iterating (node, degree)
       //     pairs
   
       //     Examples
       //     --------
       //     >>> G = xn::path_graph(4);  // or DiGraph, MultiGraph, MultiDiGraph,
       //     etc
       //     >>> G.degree[0];  // node 0 has degree 1
       //     1
       //     >>> list(G.degree([0, 1, 2]));
       //     [(0, 1), (1, 2), (2, 2)];
       //      */
       //     auto degree = DegreeView(*this);
       //     this->operator[]("degree") = std::any(degree);
       //     return degree;
       // }
   
       auto clear()
       {
           this->_adj.clear();
           // this->_node.clear();
           this->graph.clear();
       }
   
       auto is_multigraph()
       {
           return false;
       }
   
       auto is_directed()
       {
           return false;
       }
   };
   
   using SimpleGraph = Graph<decltype(py::range<int>(1)),
       decltype(py::range<int>(1)), py::set<int>>;
   // Clang 8.0 does not support the following deduction rule.
   // template <typename nodeview_t, typename nodemap_t,
   //           typename adjlist_inner_dict_factory> Graph(int )->
   //           Graph<decltype(py::range<int>(1)), decltype(py::range<int>(1)),
   //           py::set<int>>;
   
   } // namespace xn
   
   #endif
