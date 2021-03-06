#include "utils.hpp"
#include "graph.hpp"

#include <algorithm>
#include <list>
#include <set>

#include <boost/graph/connected_components.hpp>

using namespace std;

bool
includes(const SSC &a, const SSC &b)
{
  return includes(a.begin(), a.end(), b.begin(), b.end());
}

bool
includes(const SSSC &a, const SSC &b)
{
  for(SSSC::const_iterator i = a.begin(); i != a.end(); ++i)
    if (includes(*i, b))
      return true;

  return false;
}

bool
includes(const SSSC &a, const SSSC &b)
{
  bool status = true;

  for(SSSC::const_iterator ib = b.begin(); status && ib != b.end(); ++ib)
    {
      bool found = false;

      for(SSSC::const_iterator ia = a.begin(); !found && ia != a.end(); ++ia)
        if (includes(*ia, *ib))
          found = true;

      status &= found;
    }

  return status;
}

bool
excludes(const SSC &a, const SSC &b)
{
  return intersection(a, b).empty();
}

SSC
intersection(const SSC &a, const SSC &b)
{
  SSC r;

  set_intersection(a.begin(), a.end(),
                   b.begin(), b.end(),
                   inserter(r, r.begin()));

  return r;
}

SSSC
intersection(const SSSC &sssc, const SSC &ssc)
{
  SSSC r;

  for(SSSC::const_iterator i = sssc.begin(); i != sssc.end(); ++i)
    {
      SSC ssci = intersection(*i, ssc);
      if (!ssci.empty())
        r.insert(ssci);
    }

  return r;
}

SSC
exclude(const SSC &ssc, int nsc)
{
  SSC result;

  SSC::const_iterator i = ssc.begin();

  while(i != ssc.end())
    {
      SSC partial;

      // This loop builds the "partial" set with contiguous
      // subcarriers.  In an interation we examine a single
      // subcarrier.
      while(i != ssc.end())
        {
          int sc = *i;

          // The break condition for this loop, i.e. the condition to
          // stop building the partial set.  If we break the loop, we
          // don't add the sc element and don't increment i.
          if (!partial.empty() && ((*partial.rbegin() + 1) != sc))
            break;

          partial.insert(sc);
          ++i;
        }

      // Add the partial set only if has at least p subcarriers.
      // Otherwise just ignore it.
      if (partial.size() >= nsc)
        result.insert(partial.begin(), partial.end());
    }

  return result;
}

SSSC
exclude(const SSSC &sssc, int nsc)
{
  SSSC r;

  for(SSSC::const_iterator i = sssc.begin(); i != sssc.end(); ++i)
    {
      SSC c_ssc = exclude(*i, nsc);
      if (!c_ssc.empty())
        r.insert(c_ssc);
    }

  return r;
}

void
exclude(SSC &ssc, const SSC &e)
{
  for(SSC::const_iterator i = e.begin(); i != e.end(); ++i)
    ssc.erase(*i);
}

void
include(SSC &ssc, const SSC &e)
{
  for(SSC::const_iterator i = e.begin(); i != e.end(); ++i)
    ssc.insert(*i);
}

// The function for sorting the list of sets.
static bool stlos(const set<vertex> &s1, const set<vertex> &s2)
{
  return s1.size() > s2.size();
}

list<set<vertex> >
get_components(const graph &g)
{
  // An element of this vector corresponds to a vertex, i.e. c[v] is
  // the component number that vertex v belongs to.
  std::vector<int> c(num_vertices(g));
  // "num" is the number of connected components.
  int num = connected_components(g, &c[0]);

  // Each element of the list is a set that contains vertexes
  // belonging to a component.
  list<set<vertex> > l(num);
  // In every iteration add one vertex to the appropriate set.
  for (std::vector<int>::iterator i = c.begin(); i != c.end(); ++i)
    {
      list<set<vertex> >::iterator li = l.begin();
      advance(li, *i);

      // Iterator i refers to the element for vertex v.
      vertex v = distance(c.begin(), i);
      li->insert(v);
    }

  l.sort(stlos);

  return l;
}

SSSC
split(const SSC &ssc)
{
  SSSC sssc;

  for(SSC::const_iterator i = ssc.begin(); i != ssc.end();)
    {
      SSC tmp;
      int sc;

      do
        {
          sc = *i;
          tmp.insert(sc);
          ++i;
        } while (i != ssc.end() && sc + 1 == *i);

      sssc.insert(tmp);
    }

  return sssc;
}

int
calculate_fragments(const SSC &ssc)
{
  return split(ssc).size();
}
