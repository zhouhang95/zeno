#ifndef ZENO_EMBED_RUN_H
#define ZENO_EMBED_RUN_H

#include "cobalt.h"

#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <zeno/core/Session.h>
#include <zeno/core/Graph.h>

class RunCommand : public Cobalt::Command<RunCommand> {
public:
  static std::string Use() {
      return "run";
  }

  static std::string Short() {
      return "Run [ zsl file ].";
  }

  static std::string Long() {
      return "Run the given zsl file.";
  }

  void RegisterFlags() {
      AddLocalFlag<bool>(bUseFileInput, "with_file", "f", true, "Read zsl from file.");
  }

  int Run(const Cobalt::Arguments& Args) {
      // Read file
      if (bUseFileInput && !Args.empty()) {
          const std::string& FilePath = Args[0];
          std::freopen(FilePath.c_str(), "r", stdin);
      }

      // Loading json from stdin buffer
      std::ostringstream StringStream;
      StringStream << std::cin.rdbuf();
      std::string InJson { StringStream.str() };

      // Create new graph
      std::shared_ptr<zeno::Graph> NewGraph = zeno::getSession().createGraph();
      NewGraph->loadGraph(InJson.c_str());

      // Run graph
      try {
          NewGraph->applyNodesToExec();
      } catch (...) {
          std::cerr << "There is an exception happened." << std::endl;
      }

      return 0;
  }

private:
  bool bUseFileInput;
};

class RootCommand : public Cobalt::Command<RootCommand, RunCommand> {
public:
  static std::string Use() {
      return "zeno";
  }
};

#endif //ZENO_EMBED_RUN_H
