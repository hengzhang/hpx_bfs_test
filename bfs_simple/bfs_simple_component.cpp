
#include "bfs_simple_component.hpp"

HPX_REGISTER_COMPONENT_MODULE();

typedef hpx::components::managed_component<
  bfs_simple::server::point > point_type;

HPX_REGISTER_MINIMAL_COMPONENT_FACTORY(point_type, point);

HPX_REGISTER_ACTION( bfs_simple::server::point::traverse_action, 
    point_traverse_action);

HPX_REGISTER_ACTION( bfs_simple::server::point::init_action, 
    point_init_action);
