//
// Exporter.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <string>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace iqlogger::stats::telegraf {

class Exporter
{
  std::string m_measurement;

  boost::property_tree::ptree getMetricsPtree();

public:
  Exporter(const std::string& measurement);
  ~Exporter() = default;
  std::string exportTelegraf();

private:
  template<typename Tree, typename F, typename Pred /* = bool(*)(Tree const&)*/, typename PathType = std::string>
  void visit_if(Tree& tree, F const& f, Pred const& p, PathType const& path = PathType()) {
    if (p(tree))
      f(path, tree);

    for (auto& child : tree)
      if (path.empty())
        visit_if(child.second, f, p, child.first);
      else
        visit_if(child.second, f, p, path + "." + child.first);
  }

  template<typename Tree, typename F, typename PathType = std::string>
  void visit(Tree& tree, F const& f, PathType const& path = PathType()) {
    visit_if(
        tree, f, [](Tree const&) { return true; }, path);
  }

  static bool is_leaf(boost::property_tree::ptree const& pt) { return pt.empty(); }
};
}  // namespace iqlogger::stats::telegraf
