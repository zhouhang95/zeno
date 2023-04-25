
#include "include/cobalt.h"
#include "include/run.h"

int main(int argc, char** argv) {
    // Parse input args
    return Cobalt::Execute<RootCommand>(argc, argv);

//    const char *json = "json here";
//    auto g = zeno::getSession().createGraph();
//    g->addSubnetNode("custom")->loadGraph(json);
//    std::map<std::string, std::shared_ptr<zeno::IObject>> inputs, outputs;
//    outputs = g->callSubnetNode("custom", std::move(inputs));
//    return 0;
}
