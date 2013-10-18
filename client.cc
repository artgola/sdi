#include "client.hpp"
#include <iostream>

using namespace std;

client::client(pqueue &q, int id, boost::mt19937 &r,
               double l_sleep, double mnc, double l_change):
  module(q), id(id), r(r),
  idle(true), nc_left(0),
  l_sleep(l_sleep), sd(l_sleep), sdg(r, sd),
  l_change(l_change), cd(l_change), cdg(r, cd),
  mnc(mnc), nd(mnc), ndg(r, nd)
{
}

client::~client()
{
}

void client::operator()(double t)
{
  cout << "Event t = " << t << ", id = " << id << ": ";

  if (idle)
    {
      // Now the client gets busy.
      idle = false;
      nc_left = ndg();

      // Establish a new connection.
      cout << "establishing";
    }
  else
    if (nc_left)
      {
        --nc_left;

        // Reconfigure the connection.
        cout << "reconfiguring";
      }
    else
      {
        // Take the connection down.
        cout << "taking down";

        // Now the client gets idle.
        idle = true;
      }

  cout << endl;

  schedule(t);
}

// Schedule the next event based on the current time 0.
void client::schedule(double t)
{
  double dt;

  // The time to sleep.
  dt = idle ? sdg() : cdg();

  module::schedule(t + dt);
}
