#ifndef UTILS_HPP
#define UTILS_HPP

#include "graph.hpp"

#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

#include <boost/graph/dijkstra_shortest_paths.hpp>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>

#include <cassert>
#include <list>
#include <set>
#include <string>
#include <vector>
#include <set>

namespace ba = boost::accumulators;

/**
 * Generate a random number from min to max, including both min and
 * max.
 */
template<typename T>
int
get_random_int(int min, int max, T &gen)
{
  boost::uniform_int<> range(min, max);
  boost::variate_generator<T &, boost::uniform_int<> > rgen(gen, range);
  return rgen();
}

/**
 * Check whether b is a subset of a.
 */
bool
includes(const SSC &a, const SSC &b);

/**
 * Check whether b is somewhere in a.
 */
bool
includes(const SSSC &a, const SSC &b);

/**
 * Check whether b is a subset of a.
 */
bool
includes(const SSSC &a, const SSSC &b);

/**
 * Check whether b is not a subset of a.
 */
bool
excludes(const SSC &a, const SSC &b);

/**
 * SSC intersection.
 */
SSC
intersection(const SSC &s1, const SSC &s2);

/**
 * SSSC and SSC intersection.
 */
SSSC
intersection(const SSSC &sssc, const SSC &ssc);

/**
 * Exclude those subcarriers that won't be able to support a signal
 * with nsc subcarriers.
 */

SSC
exclude(const SSC &ssc, int nsc);

/**
 * Exclude those subcarriers from SSSC that won't be able to support a
 * signal with nsc subcarriers.
 */

SSSC
exclude(const SSSC &sssc, int nsc);

/**
 * Exclude e from ssc.
 */
void
exclude(SSC &ssc, const SSC &e);

/**
 * Include e to ssc.
 */
void
include(SSC &ssc, const SSC &e);

/**
 * Calculates the shortests paths in the graph.
 */

template<typename G>
void complete_graph(G &g)
{
  // Here we do all-pair shortest path.  There are two things to note.
  // 1: We could use the Floyd-Warshal or the Johnson algorithms here,
  // but they return only the distances.  They don't return the
  // parents. 2: We want to shortest paths to a destination from other
  // nodes, and the Dijksta algorithm does the opposite: it returns
  // the shortest paths from a source node to other nodes.  For us it
  // doesn't matter, because links are undirected.
  std::vector<int> dist(num_vertices(g));
  std::vector<vertex> pred(num_vertices(g));

  typename boost::graph_traits<G>::vertex_iterator vi, ve;
  for (tie(vi, ve) = vertices(g); vi != ve; ++vi)
    {
      typename boost::graph_traits<G>::vertex_descriptor v = *vi;

      boost::dijkstra_shortest_paths
        (g, v, boost::predecessor_map(&pred[0]).distance_map(&dist[0]));

      put(boost::vertex_distance, g, v, dist);
      put(boost::vertex_predecessor, g, v, pred);
    }
}

/**
 * Prints a path from node i to node j in graph g.
 */

template<typename G>
std::string
path_to_string(typename boost::graph_traits<G>::vertex_descriptor i,
               typename boost::graph_traits<G>::vertex_descriptor j,
               const G &g)
{
  std::ostringstream str;

  str << "From " << boost::get(boost::vertex_name, g, i)
      << " to " << boost::get(boost::vertex_name, g, j);

  // Check if the shortest path from node i to node j exists.
  if (i == j || boost::get(boost::vertex_predecessor, g, j)[i] != i)
    {
      int hops = 0;
      typename boost::graph_traits<G>::vertex_descriptor curr = i;

      str << ": " << boost::get(boost::vertex_name, g, i);

      while(curr != j)
        {
          curr = boost::get(boost::vertex_predecessor, g, j)[curr];
	  str << " -> " << get(boost::vertex_name, g, curr);
          ++hops;
        }

      str << ", hops = " << hops;
      str << ", distance = "
	  << boost::get(boost::vertex_distance, g, i)[j];
    }
  else
    str << "doesn't exist";

  return str.str();
}

/**
 * Prints the information on the shortest paths.
 */

template<typename G>
void
print_sp(const G &g, std::ostream &os)
{
  os << "******************************************************\n";
  os << "SHORTEST PATHS\n";
  os << "******************************************************\n";

  // Lists the path from i to j.

  typename boost::graph_traits<G>::vertex_iterator i, ie;
  typename boost::graph_traits<G>::vertex_iterator j, je;
  
  for (tie(i, ie) = vertices(g); i != ie; ++i)
    for (tie(j, je) = vertices(g); j != je; ++j)
      os << path_to_string(*i, *j, g) << std::endl;
}

/**
 * Return a container with vertexes of a graph.
 */
template<typename C>
C
get_vertexes(const graph &g)
{
  C c;

  // Copy the vertex descriptors.
  std::copy(vertices(g).first, vertices(g).second,
            inserter(c, c.begin()));

  return c;
}

/**
 * Get a random element from a container.
 */
template <typename C, typename T>
typename C::value_type
get_random_element(const C &c, T &gen)
{
  assert(!c.empty());

  boost::uniform_int<> dist(0, c.size() - 1);
  boost::variate_generator<T &, boost::uniform_int<> >
    rand_gen(gen, dist);

  typename C::const_iterator i = c.begin();
  std::advance(i, rand_gen());

  return *i;
}

/**
 * This is the << operator for a vector.
 */
template <typename T>
std::ostream &operator << (std::ostream &os, const std::vector<T> &v)
{
  typename std::vector<T>::const_iterator i = v.begin();

  os << "[";

  while(i != v.end())
    {
      os << *i;
      if (++i != v.end())
        os << ", ";
    }

  os << "]";

  return os;
}

/**
 * This is the << operator for a set.
 */
template <typename T>
std::ostream &operator << (std::ostream &os, const std::set<T> &v)
{
  typename std::set<T>::const_iterator i = v.begin();

  os << "{";

  while(i != v.end())
    {
      os << *i;
      if (++i != v.end())
        os << ", ";
    }

  os << "}";

  return os;
}

/**
 * This is the << operator for a list.
 */
template <typename T>
std::ostream &operator << (std::ostream &os, const std::list<T> &v)
{
  typename std::list<T>::const_iterator i = v.begin();

  os << "(";

  while(i != v.end())
    {
      os << *i;
      if (++i != v.end())
        os << ", ";
    }

  os << ")";

  return os;
}

/**
 * Make sure that the graph has only one connected component, and that
 * perhaps there are some lonely vertexes.  Sort the list in the
 * decreasing order of the number of elements in the sets.
 */
std::list<std::set<vertex> >
get_components(const graph &g);

/**
 * There must be only one connected component.  There could be other
 * components, but they can't be connected (i.e. they are lone
 * vertexes).
 *
 * @return: true if only one component is connected, false otherwise.
 */
template<typename G>
bool
check_components(const G &g)
{
  typedef typename boost::graph_traits<G>::vertex_descriptor vertex;
  typedef typename std::list<std::set<vertex> > lsv;

  lsv l = get_components(g);

  int connected = 0;
  // Count the number of connected components.
  for (typename lsv::iterator i = l.begin(); i != l.end(); ++i)
    connected += (i->size() >= 2);

  return connected == 1;
}

/**
 * Generate a random node pair.
 */

template<typename G, typename R>
std::pair<typename boost::graph_traits<G>::vertex_descriptor,
	  typename boost::graph_traits<G>::vertex_descriptor>
random_node_pair(const G &g, R &gen)
{
  typedef typename boost::graph_traits<G>::vertex_descriptor vertex;
  typedef typename boost::graph_traits<G>::vertex_iterator vertex_i;

  vertex_i begin = vertices(g).first;
  int n = num_vertices(g);

  int src_n = get_random_int(0, n - 1, gen);
  vertex_i src_i = begin;
  std::advance(src_i, src_n);
  vertex src = *src_i;

  // Choose any number between 0 and n - 1, but not src_n.
  int dst_n = get_random_int(0, n - 2, gen);
  if (dst_n == src_n)
    dst_n = n - 1;
  vertex_i dst_i = begin;
  std::advance(dst_i, dst_n);
  vertex dst = *dst_i;

  return std::make_pair(src, dst);
}

template<typename G>
double
calculate_load(const G &g)
{
  ba::accumulator_set<double, ba::stats<ba::tag::mean> > load_acc;

  typename boost::graph_traits<G>::edge_iterator ei, ee;
  for (tie(ei, ee) = edges(g); ei != ee; ++ei)
    {
      // Available subcarriers.
      int tsc = boost::get(boost::edge_nosc, g, *ei);
      // Available subcarriers.
      int asc = boost::get(boost::edge_ssc, g, *ei).size();
      // The link load.
      double load = double(tsc - asc) / tsc;
      load_acc(load);
    }

  return ba::mean(load_acc);
}

SSSC
split(const SSC &ssc);

int
calculate_fragments(const SSC &ssc);

#endif /* UTILS_HPP */
