

#if !defined(BFS_SIMPLE_HPP)
#define BFS_SIMPLE_HPP

#include <vector>
#include <iostream>
#include <hpx/include/components.hpp>
#include <hpx/runtime/components/server/managed_component_base.hpp>
#include <hpx/runtime/actions/component_action.hpp>
 #include <hpx/runtime/components/stubs/stub_base.hpp>
 #include <hpx/include/async.hpp> 
#include <hpx/runtime/applier/applier.hpp>
#include <hpx/include/client.hpp>
#include <hpx/runtime/components/stubs/runtime_support.hpp> 

#include <hpx/hpx_fwd.hpp>
#include <hpx/include/serialization.hpp>
#include <hpx/include/lcos.hpp> 
#include <hpx/include/actions.hpp>  


#include <boost/serialization/vector.hpp>

namespace bfs_simple{
  namespace server{
    class HPX_COMPONENT_EXPORT point: public hpx::components::managed_component_base<point>
    {
     // typedef std::size_t argument_type_size; 
      //typedef std::vector<argument_type_size> argument_type;
      private:
        std::size_t id;
        std::vector<std::size_t> neighbors;
        std::size_t level;
        std::size_t parent;
        bool visited;
        
      public:
        //why need to add this construct function?
        point(){}

        /*point(std::size_t id_in, std::vector<std::size_t> neighbors_in):
          id(id_in){
            neighbors.assign(neighbors_in.begin(), neighbors_in.end());
          }*/
        //add init function
        void init(std::size_t id_in, std::vector<std::size_t>  neighbors_in){
            id = id_in;
            neighbors.assign(neighbors_in.begin(), neighbors_in.end());
        }
        ///////
        std::vector<std::size_t> traverse(std::size_t level_in, std::size_t parent_in){
          if(visited == false){
            visited = true;
            parent = parent_in;
            level = level_in;
            std::cout <<(boost::format("point_id %1$, parent_id %2%, level %3%\n") % id % parent % level);
            return neighbors;
          }else{
            std::vector<std::size_t> tmp; 
            return tmp;
          }
        }
        HPX_DEFINE_COMPONENT_ACTION(point , traverse);
        HPX_DEFINE_COMPONENT_ACTION(point , init);
    };
  }

  namespace stubs{
   struct point : hpx::components::stub_base<server::point>{
     static void init(hpx::naming::id_type gid,std::size_t id_in, std::vector<std::size_t> neighbors_in){
      hpx::apply<server::point::init_action>(gid,id_in, neighbors_in); 
     } 
     static  hpx::lcos::promise<std::vector<std::size_t> > traverse_async(hpx::naming::id_type const& gid_in, std::size_t level_in, std::size_t parent_in){
       typedef server::point::traverse_action action_type;
       return  hpx::lcos::packaged_action<action_type>(gid_in, level_in, parent_in);
     }
     static  std::vector<std::size_t> traverse(hpx::naming::id_type const& gid_in, std::size_t level_in, std::size_t parent_in){
       return traverse_async(gid_in, level_in, parent_in).get_future().get();
     }
   };
  }


  class point : hpx::components::client_base<point , stubs::point>{
   typedef hpx::components::client_base<point, stubs::point>  base_type;

    public:
   point(){}
 //  point(hpx::shared_future<hpx::naming::id_type> const&gid, std::size_t id_in, std::vector<std::size_t> neighbors_in):base_type(gid)  {}

  // point(hpx::naming::id_type const&gid, std::size_t id_in, std::vector<std::size_t> neighbors_in):base_type(gid)  {}

   point(hpx::naming::id_type const&gid):base_type(gid)  {}

   void init(std::size_t id_in, std::vector<std::size_t> neighbors_in){
    HPX_ASSERT(this->get_gid());
    this->base_type::init(this->get_gid(),id_in, neighbors_in);
   }

   hpx::lcos::promise<std::vector<std::size_t> > traverse_async(std::size_t level_in, std::size_t parent_in){
     HPX_ASSERT(this->get_gid()); 
     return this->base_type::traverse_async(this->get_gid(), level_in, parent_in);
   }
   std::vector<std::size_t> traverse(std::size_t level_in, std::size_t parent_in){
     HPX_ASSERT(this->get_gid()); 
     return this->base_type::traverse(this->get_gid(), level_in, parent_in);
   }
  };

}
HPX_REGISTER_ACTION_DECLARATION(bfs_simple::server::point::traverse_action, point_traverse_action);

HPX_REGISTER_ACTION_DECLARATION(bfs_simple::server::point::init_action, point_init_action);

#endif
