#include "utils.hpp"
#include "graph.hpp"

#include <list>
#include <set>

#include <boost/graph/connected_components.hpp>

using namespace std;

// The function for sorting the list of sets.
static bool stlos(const set<Vertex> &s1, const set<Vertex> &s2)
{
  return s1.size() > s2.size();
}

list<set<Vertex> >
get_components(const Graph &g)
{
  // An element of this vector corresponds to a vertex, i.e. c[v] is
  // the component number that vertex v belongs to.
  std::vector<int> c(num_vertices(g));
  // "num" is the number of connected components.
  int num = connected_components(g, &c[0]);

  // Each element of the list is a set that contains vertexes
  // belonging to a component.
  list<set<Vertex> > l(num);
  // In every iteration add one vertex to the appropriate set.
  for (std::vector<int>::iterator i = c.begin(); i != c.end(); ++i)
    {
      list<set<Vertex> >::iterator li = l.begin();
      advance(li, *i);

      // Iterator i refers to the element for vertex v.
      Vertex v = distance(c.begin(), i);
      li->insert(v);
    }

  l.sort(stlos);

  return l;
}