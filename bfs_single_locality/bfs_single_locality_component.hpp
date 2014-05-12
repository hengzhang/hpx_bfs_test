

#if !defined(BFS_SIMPLE_HPP)
#define BFS_SIMPLE_HPP

#include <vector>
#include <queue>
#include <iostream>
#include <hpx/include/components.hpp>
#include <hpx/runtime/components/server/managed_component_base.hpp>
#include <hpx/runtime/actions/component_action.hpp>
 #include <hpx/runtime/components/stubs/stub_base.hpp>
#include <hpx/runtime/applier/applier.hpp>
 #include <hpx/runtime/actions/component_action.hpp>
#include <hpx/include/client.hpp>
 #include <hpx/runtime/components/client_base.hpp>
#include <hpx/hpx_fwd.hpp> 
#include <hpx/include/async.hpp>


#include <boost/serialization/vector.hpp>

namespace bfs_single_locality{
  namespace server{
    class HPX_COMPONENT_EXPORT graph: public hpx::components::managed_component_base<graph>
    {
     // typedef std::size_t argument_type_size; 
      //typedef std::vector<argument_type_size> argument_type;
      private:
        std::size_t id;
        std::vector<std::vector<std::size_t> > neighbors;
        std::vector<std::size_t> parents;
        
      public:
        graph():id(0){}

        //add init function
        void init(std::size_t id_in, std::vector<std::pair<std::size_t, std::size_t> >  const & edgelist, std::size_t vertex_num){
            id = id_in;
            parents.resize(vertex_num);
            std::fill(parents.begin(), parents.end(), 0);
            
            neighbors.resize(vertex_num);
            for(std::size_t i =0;i<edgelist.size(); ++i){
              std::size_t node = edgelist[i].first;
              std::size_t neighbor = edgelist[i].second;
              
              if(node != neighbor){
                neighbors[node].push_back(neighbor);
                // directed graph
                //neighbors[neighbor].push_back(node);
              }
            }
        }
        std::vector< std::size_t> bfs_graph(std::size_t root){
          //hpx::util::high_resolution_timer t;
          std::queue<std::size_t> q;
          std::fill(parents.begin(), parents.end(), 0);
          parents[root] = root;
          q.push(root);
          while( !q.empty()){
            std::size_t node = q.front(); q.pop();
            std::vector< std::size_t> const & node_neighbors = neighbors[node];
            for(std::vector<std::size_t>::const_iterator it = node_neighbors.begin(); it != node_neighbors.end(); ++it){
              std::size_t neighbor = *it;
              std::size_t& neighbor_parent = parents[neighbor];
              if(std::size_t(0) == neighbor_parent){
                neighbor_parent = node;
                q.push(neighbor);
              }
              //[debug::
            /*  for(std::size_t j =0;j < parents.size();++j){
                std::cout<< parents[j]<<" ";
              }*/
              //!!!]

            }
          }
          return parents;
        }
        HPX_DEFINE_COMPONENT_ACTION(graph, bfs_graph);
        HPX_DEFINE_COMPONENT_ACTION(graph, init);
    };
  }

  namespace stubs{
   struct graph: hpx::components::stub_base<server::graph>{

     static hpx::lcos::future<void> init_async(hpx::naming::id_type const& gid,std::size_t id_in, std::vector<std::pair<std::size_t,std::size_t> > const & edgelist_in, std::size_t vertex_num){
       typedef server::graph::init_action action_type;
       return hpx::async<action_type >(gid,id_in, edgelist_in, vertex_num); 
     } 

     static void init(hpx::naming::id_type const& gid, std::size_t id_in, std::vector<std::pair<std::size_t, std::size_t> > const& edgelist_in, std::size_t vertex_num){
      init_async(gid, id_in, edgelist_in, vertex_num); 
     }

     static  hpx::lcos::future<std::vector<std::size_t> > bfs_graph_async(hpx::naming::id_type const& gid_in, std::size_t root_in){
       typedef server::graph::bfs_graph_action action_type;
       return  hpx::async<action_type >(gid_in, root_in);
     }

     static  std::vector<std::size_t> bfs_graph(hpx::naming::id_type const& gid_in, std::size_t root_in){
       return bfs_graph_async(gid_in, root_in).get();
     }
   };
  }


  class graph: public hpx::components::client_base<graph, stubs::graph>{
    typedef hpx::components::client_base<graph, stubs::graph>  base_type;
    public:
    graph(){}
    graph(hpx::future<hpx::naming::id_type> && gid):base_type(std::move(gid))  {}
    hpx::lcos::future<void> init_async(std::size_t id_in, std::vector<std::pair<std::size_t, std::size_t> > const & edgelist_in, std::size_t vertex_num){
      HPX_ASSERT(this->get_gid());
      return this->base_type::init_async(this->get_gid(), id_in, edgelist_in, vertex_num);
    }
    void init(std::size_t id_in, std::vector<std::pair< std::size_t, std::size_t> > const& edgelist_in, std::size_t vertex_num){
      HPX_ASSERT(this->get_gid());
      this->base_type::init(this->get_gid(),id_in, edgelist_in, vertex_num);
    }

    hpx::lcos::future<std::vector<std::size_t> > bfs_graph_async(std::size_t root_in){
      HPX_ASSERT(this->get_gid()); 
      return this->base_type::bfs_graph_async(this->get_gid(), root_in);
    }
    std::vector<std::size_t> bfs_graph(std::size_t root_in){
      HPX_ASSERT(this->get_gid()); 
      return this->base_type::bfs_graph(this->get_gid(), root_in);
    }
  };
}
HPX_REGISTER_ACTION_DECLARATION(bfs_single_locality::server::graph::bfs_graph_action, graph_bfs_graph_action);

HPX_REGISTER_ACTION_DECLARATION(bfs_single_locality::server::graph::init_action, graph_init_action);

#endif
