
.. _program_listing_file__media_lubuntu_USBDISK_github_ckpttncpp_lib_include_py2cpp_nx2bgl.hpp:

Program Listing for File nx2bgl.hpp
===================================

|exhale_lsh| :ref:`Return to documentation for file <file__media_lubuntu_USBDISK_github_ckpttncpp_lib_include_py2cpp_nx2bgl.hpp>` (``/media/lubuntu/USBDISK/github/ckpttncpp/lib/include/py2cpp/nx2bgl.hpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   // -*- coding: utf-8 -*-
   #pragma once
   
   #include <boost/graph/adjacency_list.hpp>
   #include <boost/graph/graph_traits.hpp>
   #include <boost/graph/graph_utility.hpp>
   #include <type_traits>
   
   namespace xn
   {
   
   template <typename Graph>
   class VertexView : public Graph
   {
     public:
       explicit VertexView(Graph& G)
           : Graph(G)
       {
       }
   
       auto begin()
       {
           auto [v_iter, v_end] = boost::vertices(*this);
           return v_iter;
       }
   
       auto end()
       {
           auto [v_iter, v_end] = boost::vertices(*this);
           return v_end;
       }
   
       auto cbegin() const
       {
           auto [v_iter, v_end] = boost::vertices(*this);
           return v_iter;
       }
   
       auto cend() const
       {
           auto [v_iter, v_end] = boost::vertices(*this);
           return v_end;
       }
   };
   
   template <typename Graph>
   class EdgeView
   {
     private:
       const Graph& _G;
   
     public:
       explicit EdgeView(const Graph& G)
           : _G {G}
       {
       }
   
       auto begin() const
       {
           auto [e_iter, e_end] = boost::edges(_G);
           return e_iter;
       }
   
       auto end() const
       {
           auto [e_iter, e_end] = boost::edges(_G);
           return e_end;
       }
   
       auto cbegin() const
       {
           auto [e_iter, e_end] = boost::edges(_G);
           return e_iter;
       }
   
       auto cend() const
       {
           auto [e_iter, e_end] = boost::edges(_G);
           return e_end;
       }
   };
   
   template <typename Vertex, typename Graph>
   class AtlasView
   {
     private:
       Vertex _v;
       const Graph& _G;
   
     public:
       explicit AtlasView(Vertex v, const Graph& G)
           : _v {v}
           , _G {G}
       {
       }
   
       auto begin() const
       {
           auto [e_iter, e_end] = boost::out_edges(_v, _G);
           return e_iter;
       }
   
       auto end() const
       {
           auto [e_iter, e_end] = boost::out_edges(_v, _G);
           return e_end;
       }
   
       auto cbegin() const
       {
           auto [e_iter, e_end] = boost::out_edges(_v, _G);
           return e_iter;
       }
   
       auto cend() const
       {
           auto [e_iter, e_end] = boost::out_edges(_v, _G);
           return e_end;
       }
   };
   
   template <typename Graph>
   class grAdaptor : public VertexView<Graph>
   {
     public:
       using Vertex = typename boost::graph_traits<Graph>::vertex_descriptor;
       using node_t = Vertex;
       using edge_t = typename boost::graph_traits<Graph>::edge_descriptor;
   
       // using edge_wt_t = decltype( boost::get(boost::edge_weight,
       // std::declval<Graph>()) );
   
       explicit grAdaptor() = delete;
   
       explicit grAdaptor(Graph& G)
           : VertexView<Graph>(G)
       {
       }
   
       auto number_of_edges() const
       {
           return boost::num_edges(*this);
       }
   
       EdgeView<Graph> edges() const
       {
           return EdgeView<Graph>(*this);
       }
   
       AtlasView<Vertex, Graph> neighbors(Vertex v) const
       {
           return AtlasView<Vertex, Graph>(v, *this);
       }
   
       auto add_edge(int u, int v)
       {
           return boost::add_edge(u, v, *this);
       }
   
       static Vertex null_vertex()
       {
           return boost::graph_traits<Graph>::null_vertex();
       }
   
       template <typename Edge>
       Vertex source(const Edge& e) const
       {
           return boost::source(e, *this);
       }
   
       template <typename Edge>
       Vertex target(const Edge& e) const
       {
           return boost::target(e, *this);
       }
   
       template <typename Edge>
       auto end_points(const Edge& e) const
       {
           auto s = boost::source(e, *this);
           auto t = boost::target(e, *this);
           return std::pair {s, t};
       }
   };
   
   } // namespace xn
