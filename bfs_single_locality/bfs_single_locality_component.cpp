
#include "bfs_single_locality_component.hpp"
#include <hpx/hpx.hpp> 
#include <hpx/runtime/components/component_factory.hpp> 

HPX_REGISTER_COMPONENT_MODULE();

typedef hpx::components::managed_component<
  bfs_single_locality::server::graph > graph_type;

HPX_REGISTER_MINIMAL_COMPONENT_FACTORY(graph_type, graph);

HPX_REGISTER_ACTION( bfs_single_locality::server::graph::bfs_graph_action, 
    graph_bfs_graph_action);

HPX_REGISTER_ACTION( bfs_single_locality::server::graph::init_action, 
    graph_init_action);
