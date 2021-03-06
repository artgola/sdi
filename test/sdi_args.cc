#include "sdi_args.hpp"
#include "utils.hpp"
#include "utils_netgen.hpp"
#include "dijkstra.hpp"

#include <iostream>

#define BOOST_TEST_MODULE Arguments

#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

/*
 * Test that the arguments are interpreted right with the default seed
 * value of 1.
 */
BOOST_AUTO_TEST_CASE(sdi_args_test_1)
{
  const char *argv[] = {"",
                        "--network", "random",
                        "--nodes", "10",
                        "--edges", "30",
                        "--subcarriers", "50",
                        "--clients", "100",
                        "--l_sleep", "11",
                        "--mnc", "12",
                        "--l_change", "13",
                        "--mnsc", "5",
                        "--reconf", "incremental",
                        "--select", "first",
                        "--max_len", "1000",
                        "--hash", "blablabla"};

  int argc = sizeof(argv) / sizeof(char *);

  sdi_args args = process_sdi_args(argc, argv);

  BOOST_CHECK(args.nr_nodes == 10);
  BOOST_CHECK(args.nr_edges == 30);
  BOOST_CHECK(args.nr_sc == 50);
  BOOST_CHECK(args.nr_clients == 100);
  BOOST_CHECK(args.hash == "blablabla");
  BOOST_CHECK(args.reconf == connection::incremental);
  BOOST_CHECK(args.select == dijkstra::get_select());

  BOOST_CHECK_CLOSE(args.l_sleep, 11, 0.0001);
  BOOST_CHECK_CLOSE(args.mnc, 12, 0.0001);
  BOOST_CHECK_CLOSE(args.l_change, 13, 0.0001);
  BOOST_CHECK_CLOSE(args.mnsc, 5, 0.0001);

  BOOST_CHECK(args.max_len == 1000);

  BOOST_CHECK(args.seed == 1);
}

/*
 * Test that the arguments are interpreted right with the default seed
 * value of 1.
 */
BOOST_AUTO_TEST_CASE(sdi_args_test_2)
{
  const char *argv[] = {"",
                        "--network", "random",
                        "--nodes", "10",
                        "--edges", "30",
                        "--subcarriers", "50",
                        "--clients", "100",
                        "--l_sleep", "11",
                        "--mnc", "12",
                        "--l_change", "13",
                        "--mnsc", "5",
                        "--reconf", "complete",
                        "--select", "fittest",
                        "--hash", "blablabla",
                        "--max_len", "1000",
                        "--seed", "2"};

  int argc = sizeof(argv) / sizeof(char *);

  sdi_args args = process_sdi_args(argc, argv);

  BOOST_CHECK(args.seed == 2);
  BOOST_CHECK(args.reconf == connection::complete);
  BOOST_CHECK(args.select == dijkstra::fittest);
}

/*
 * Test that the reconf argument "curtailing" is recognized correctly.
 */
BOOST_AUTO_TEST_CASE(sdi_args_test_3)
{
  const char *argv[] = {"",
                        "--network", "random",
                        "--nodes", "10",
                        "--edges", "30",
                        "--subcarriers", "50",
                        "--clients", "100",
                        "--l_sleep", "11",
                        "--mnc", "12",
                        "--l_change", "13",
                        "--mnsc", "5",
                        "--reconf", "curtailing",
                        "--select", "first",
                        "--hash", "blablabla",
                        "--max_len", "1000",
                        "--seed", "2"};

  int argc = sizeof(argv) / sizeof(char *);

  sdi_args args = process_sdi_args(argc, argv);

  BOOST_CHECK(args.reconf == connection::curtailing);
  BOOST_CHECK(args.select == dijkstra::first);
}
