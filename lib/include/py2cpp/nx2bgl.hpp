#ifndef PY2CPP_NX2BGL_HPP
#define PY2CPP_NX2BGL_HPP 1

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/graph_utility.hpp>
#include <type_traits>

namespace xn
{

/**
 * @brief VertexView
 *
 * @tparam Graph
 */
template <typename Graph>
class VertexView : public Graph
{
  public:
    /**
     * @brief Construct a new Vertex View object
     *
     * @param G
     */
    explicit VertexView(Graph&& G)
        : Graph {std::move(G)}
    {
    }

    /**
     * @brief begin()
     *
     * @return auto
     */
    auto begin() const
    {
        auto [v_iter, v_end] = boost::vertices(*this);
        return v_iter;
    }

    /**
     * @brief end()
     *
     * @return auto
     */
    auto end() const
    {
        auto [v_iter, v_end] = boost::vertices(*this);
        return v_end;
    }

    /**
     * @brief cbegin()
     *
     * @return auto
     */
    auto cbegin() const
    {
        auto [v_iter, v_end] = boost::vertices(*this);
        return v_iter;
    }

    /**
     * @brief cend()
     *
     * @return auto
     */
    auto cend() const
    {
        auto [v_iter, v_end] = boost::vertices(*this);
        return v_end;
    }
};

/**
 * @brief EdgeView
 *
 * @tparam Graph
 */
template <typename Graph>
class EdgeView
{
  private:
    const Graph& _G;

  public:
    /**
     * @brief Construct a new Edge View object
     *
     * @param G
     */
    explicit EdgeView(const Graph& G)
        : _G {G}
    {
    }

    /**
     * @brief begin()
     *
     * @return auto
     */
    auto begin() const
    {
        auto [e_iter, e_end] = boost::edges(_G);
        return e_iter;
    }

    /**
     * @brief end()
     *
     * @return auto
     */
    auto end() const
    {
        auto [e_iter, e_end] = boost::edges(_G);
        return e_end;
    }

    /**
     * @brief cbegin()
     *
     * @return auto
     */
    auto cbegin() const
    {
        auto [e_iter, e_end] = boost::edges(_G);
        return e_iter;
    }

    /**
     * @brief cend()
     *
     * @return auto
     */
    auto cend() const
    {
        auto [e_iter, e_end] = boost::edges(_G);
        return e_end;
    }
};

/**
 * @brief AtlasView
 *
 * @tparam Vertex
 * @tparam Graph
 */
template <typename Vertex, typename Graph>
class AtlasView
{
  private:
    Vertex _v;
    const Graph& _G;

  public:
    /**
     * @brief Construct a new Atlas View object
     *
     * @param v
     * @param G
     */
    explicit AtlasView(Vertex v, const Graph& G)
        : _v {v}
        , _G {G}
    {
    }

    /**
     * @brief begin()
     *
     * @return auto
     */
    auto begin()
    {
        auto [v_iter, v_end] = boost::adjacent_vertices(_v, _G);
        return v_iter;
    }

    /**
     * @brief end()
     *
     * @return auto
     */
    auto end()
    {
        auto [v_iter, v_end] = boost::adjacent_vertices(_v, _G);
        return v_end;
    }

    /**
     * @brief cbegin()
     *
     * @return auto
     */
    auto cbegin() const
    {
        auto [v_iter, v_end] = boost::adjacent_vertices(_v, _G);
        return v_iter;
    }

    /**
     * @brief cend()
     *
     * @return auto
     */
    auto cend() const
    {
        auto [v_iter, v_end] = boost::adjacent_vertices(_v, _G);
        return v_end;
    }
};

/**
 * @brief grAdaptor
 *
 * @tparam Graph
 */
template <typename Graph>
class grAdaptor : public VertexView<Graph>
{
  public:
    using Vertex = typename boost::graph_traits<Graph>::vertex_descriptor;

    /**
     * @brief Construct a new gr Adaptor object
     *
     */
    explicit grAdaptor() = delete;

    /**
     * @brief Construct a new gr Adaptor object
     *
     * @param G
     */
    explicit grAdaptor(Graph&& G)
        : VertexView<Graph> {std::forward<Graph>(G)}
    {
    }

    /**
     * @brief number of nodes
     *
     * @return index_t
     */
    auto number_of_nodes() const -> size_t
    {
        return boost::num_vertices(*this);
    }

    /**
     * @brief number of edges
     *
     * @return index_t
     */
    auto number_of_edges() const -> size_t
    {
        return boost::num_edges(*this);
    }

    /**
     * @brief
     *
     * @return EdgeView<Graph>
     */
    // EdgeView<Graph> edges() const { return EdgeView<Graph>(*this); }

    /**
     * @brief
     *
     * @param v
     * @return AtlasView<Vertex, Graph>
     */
    AtlasView<Vertex, Graph> neighbors(Vertex v) const
    {
        return AtlasView<Vertex, Graph>(v, *this);
    }

    /**
     * @brief
     *
     * @param v
     * @return AtlasView<Vertex, Graph>
     */
    AtlasView<Vertex, Graph> operator[](Vertex v) const
    {
        return this->neighbors(v);
    }

    /**
     * @brief
     *
     * @param u
     * @param v
     * @return auto
     */
    auto add_edge(int u, int v)
    {
        return boost::add_edge(u, v, *this);
    }

    /**
     * @brief
     *
     * @return Vertex
     */
    static Vertex null_vertex()
    {
        return boost::graph_traits<Graph>::null_vertex();
    }

    /**
     * @brief
     *
     * @tparam Edge
     * @param e
     * @return Vertex
     */
    template <typename Edge>
    Vertex source(const Edge& e) const
    {
        return boost::source(e, *this);
    }

    /**
     * @brief
     *
     * @tparam Edge
     * @param e
     * @return Vertex
     */
    template <typename Edge>
    Vertex target(const Edge& e) const
    {
        return boost::target(e, *this);
    }

    /**
     * @brief
     *
     * @param v
     * @return auto
     */
    auto out_degree(Vertex v) const
    {
        return boost::out_degree(v, *this);
    }

    /**
     * @brief
     *
     * @param v
     * @return auto
     */
    auto in_degree(Vertex v) const
    {
        return boost::in_degree(v, *this);
    }

    /**
     * @brief
     *
     * @param v
     * @return auto
     */
    auto degree(Vertex v) const
    {
        return boost::degree(v, *this);
    }

    /**
     * @brief
     *
     * @tparam Edge
     * @param e
     * @return auto
     */
    template <typename Edge>
    auto end_points(const Edge& e) const
    {
        auto s = boost::source(e, *this);
        auto t = boost::target(e, *this);
        return std::pair {s, t};
    }
};

} // namespace xn
#endif
