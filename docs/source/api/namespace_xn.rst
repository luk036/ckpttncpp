
.. _namespace_xn:

Namespace xn
============


.. contents:: Contents
   :local:
   :backlinks: none




Detailed Description
--------------------

View Classes provide node, edge && degree "views" of a graph.
Views for nodes, edges && degree are provided for all base graph classes. A view means a read-only object that is quick to create, automatically updated when the graph changes, && provides basic access like ``n : V``, ``for n : V``, ``V[n]`` && sometimes set operations.
The views are read-only iterable containers that are updated as the graph is updated. As with dicts, the graph should not be updated while (iterating through the view. Views can be iterated multiple times.
Edge && Node views also allow data attribute lookup. The resulting attribute dict is writable as ``G.edges[3, 4]["color"]="red"`` Degree views allow lookup of degree values for single nodes. Weighted degree is supported with the ``weight`` argument.
:ref:`exhale_class_classxn_1_1NodeView` 

`V = G.nodes` (or `V = G.nodes()`) allows `len(V)`, `n : V`, set
operations e.g. "G.nodes & H.nodes", && `dd = G.nodes[n]`, where
`dd` is the node data dict. Iteration is over the nodes by default.

NodeDataView 

To iterate over (node, data) pairs, use arguments to `G.nodes()`
to create a DataView e.g. `DV = G.nodes(data="color", default="red")`.
The DataView iterates as `for n, color : DV` && allows
`(n, "red"] : DV`. Using `DV = G.nodes(data=true)`, the DataViews
use the full datadict : writeable form also allowing contain testing as
`(n, {"color": "red"}] : VD`. DataViews allow set operations when
data attributes are hashable.

DegreeView 

`V = G.degree` allows iteration over (node, degree) pairs as well
as lookup: `deg=V[n]`. There are many flavors of DegreeView
for (auto In/Out/Directed/Multi. For Directed Graphs, `G.degree`
counts both : && out going edges. `G.out_degree` &&
`G.in_degree` count only specific directions.
Weighted degree using edge data attributes is provide via
`V = G.degree(weight="attr_name")` where any string with the
attribute name can be used. `weight=None` is the default.
No set operations are implemented for degrees, use NodeView.

The argument `nbunch` restricts iteration to nodes : nbunch.
The DegreeView can still lookup any node even if (nbunch is specified.

:ref:`exhale_class_classxn_1_1EdgeView` 

`V = G.edges` or `V = G.edges()` allows iteration over edges as well as
`e : V`, set operations && edge data lookup `dd = G.edges[2, 3]`.
Iteration is over 2-tuples `(u, v)` for Graph/DiGraph. For multigraphs
edges 3-tuples `(u, v, key)` are the default but 2-tuples can be obtained
via `V = G.edges(keys=false)`.

Set operations for directed graphs treat the edges as a set of 2-tuples.
For undirected graphs, 2-tuples are not a unique representation of edges.
So long as the set being compared to contains unique representations
of its edges, the set operations will act as expected. If the other
set contains both `(0, 1)` && `(1, 0)` however, the result of set
operations may contain both representations of the same edge.

EdgeDataView 

Edge data can be reported using an EdgeDataView typically created
by calling an EdgeView: `DV = G.edges(data="weight", default=1)`.
The EdgeDataView allows iteration over edge tuples, membership checking
but no set operations.

Iteration depends on `data` && `default` && for multigraph `keys`
If `data == false` (the default) then iterate over 2-tuples `(u, v)`.
If `data is true` iterate over 3-tuples `(u, v, datadict)`.
Otherwise iterate over `(u, v, datadict.get(data, default))`.
For Multigraphs, if (`keys is true`, replace `u, v` with `u, v, key`
to create 3-tuples && 4-tuples.

The argument `nbunch` restricts edges to those incident to nodes : nbunch.
Exceptions
Base exceptions && errors for XNetwork. 
 



Classes
-------


- :ref:`exhale_struct_structxn_1_1AmbiguousSolution`

- :ref:`exhale_struct_structxn_1_1ExceededMaxIterations`

- :ref:`exhale_struct_structxn_1_1HasACycle`

- :ref:`exhale_struct_structxn_1_1NodeNotFound`

- :ref:`exhale_struct_structxn_1_1object`

- :ref:`exhale_struct_structxn_1_1XNetworkAlgorithmError`

- :ref:`exhale_struct_structxn_1_1XNetworkError`

- :ref:`exhale_struct_structxn_1_1XNetworkException`

- :ref:`exhale_struct_structxn_1_1XNetworkNoCycle`

- :ref:`exhale_struct_structxn_1_1XNetworkNoPath`

- :ref:`exhale_struct_structxn_1_1XNetworkNotImplemented`

- :ref:`exhale_struct_structxn_1_1XNetworkPointlessConcept`

- :ref:`exhale_struct_structxn_1_1XNetworkUnbounded`

- :ref:`exhale_struct_structxn_1_1XNetworkUnfeasible`

- :ref:`exhale_class_classxn_1_1AtlasView`

- :ref:`exhale_class_classxn_1_1EdgeView`

- :ref:`exhale_class_classxn_1_1grAdaptor`

- :ref:`exhale_class_classxn_1_1Graph`

- :ref:`exhale_class_classxn_1_1NodeView`

- :ref:`exhale_class_classxn_1_1VertexView`


Typedefs
--------


- :ref:`exhale_typedef_namespacexn_1aaf2bd524584cfeb881cb86dcfdcd3fd2`


Variables
---------


- :ref:`exhale_variable_namespacexn_1ae3cb5953d02f34b9e32bed3fc09fa56d`
