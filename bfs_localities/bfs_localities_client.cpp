#include "bfs_localities_component.hpp"
#include <hpx/hpx.hpp>
#include <hpx/hpx_init.hpp>
#include <hpx/lcos/future_wait.hpp>
#include <hpx/components/distributing_factory/distributing_factory.hpp>
#include <hpx/include/actions.hpp>
#include <hpx/include/lcos.hpp>
#include <hpx/util/unwrapped.hpp>
#include <boost/ref.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <hpx/lcos/wait_all.hpp>
#include <boost/foreach.hpp>
#include <time.h>

#include <fstream>
#include <iostream>
#include <vector>

inline void 
init(hpx::components::server::distributing_factory::iterator_range_type r,  std::vector<hpx::naming::id_type>& p){
  BOOST_FOREACH(hpx::naming::id_type const& id, r)
    p.push_back(id);
}
inline std::size_t max_node(std::size_t n1, std::pair<std::size_t, std::size_t> const& p){
  return (std::max)((std::max)(n1, p.first), p.second);
}

inline bool read_edge_list(std::string const& graphfile,std::vector<std::pair<std::size_t, std::size_t> >& edgelist){
  std::ifstream edges(graphfile.c_str());
  if (edges.is_open()) {
    std::size_t node, neighbor;
    while (edges >> node >> neighbor)
      edgelist.push_back(std::make_pair(node+1, neighbor+1));
    return edges.eof();
  }
  std::cerr << " File " << graphfile
    << " not found! Exiting... " << std::endl;
  return false;
}
inline bool read_search_node_list(std::string const& searchfile, std::vector<std::size_t>& searchroots){
  std::ifstream nodes(searchfile.c_str());
  if (nodes.is_open()) {
    std::size_t root;
    while (nodes >> root)
      searchroots.push_back(root+1);
    return nodes.eof();
  }
  std::cerr << " File " << searchfile
    << " not found! Exiting... " << std::endl;
  return false;
}

int hpx_main(boost::program_options::variables_map& vm){
  {
    hpx::util::high_resolution_timer t_wall;

    std::string const search_file = vm["search_graph"].as<std::string>();
    std::string const edge_file = vm["graph"].as<std::string>();

    hpx::util::high_resolution_timer t_io;
    std::vector<std::pair<std::size_t, std::size_t> > edgelist;
    if (!read_edge_list(edge_file, edgelist))
      return -1;
    std::vector<std::size_t> searchroots;
    if (!read_search_node_list(search_file, searchroots))
      return -1;
    std::cout << "Elapsed time during read: " << t_io.elapsed()<< " (s)"<<std::endl;

    std::size_t vertex_num = std::accumulate(edgelist.begin(), edgelist.end(), std::size_t(0), max_node) +1;

    std::vector<std::vector<std::size_t> >  node_neighbors(vertex_num);
    for(std::size_t i=0;i<edgelist.size();++i){
      std::size_t node = boost::lexical_cast<std::size_t>(edgelist[i].first);
      std::size_t neighbor = boost::lexical_cast<std::size_t>(edgelist[i].second);
      node_neighbors[node].push_back(neighbor);
    }

    std::cout<<"vertex_num :"<<vertex_num<<std::endl;

    std::cout<<"Edge list :"<<std::endl;
    for(std::size_t i=0;i<edgelist.size();++i){
      std::cout<< edgelist[i].first<<" "<<edgelist[i].second<<std::endl;
    }
    std::cout<<"Search node:"<<std::endl;
    for(std::size_t i=0;i<searchroots.size();++i){
      std::cout<< searchroots[i]<<std::endl;
    }

    hpx::components::distributing_factory factory =  hpx::components::distributing_factory::create(hpx::find_here());
    hpx::components::component_type block_type = bfs_localities::server::point::get_component_type();

    std::vector<hpx::naming::id_type> localities = hpx::find_all_localities(block_type);
     std::cout << " Number of localities: " << localities.size() << std::endl;

    hpx::components::distributing_factory::result_type blocks =
      factory.create_components(block_type,  vertex_num);

    std::vector<hpx::naming::id_type> points;
    init(hpx::util::locality_results(blocks), points);

    hpx::util::high_resolution_timer t_init_point;
    {
      std::vector<hpx::lcos::future<void> > init_phase;
      bfs_localities::server::point::init_action  init;
      for(std::size_t i =0; i<vertex_num;i++){
        init_phase.push_back(hpx::async(init, points[i], i, node_neighbors[i]));
      }
      hpx::wait_all(init_phase);

      for(std::size_t i=0;i<vertex_num;i++){
      }
    }
    double time_init_point= t_init_point.elapsed();
    std::cout << "Elapsed time during init point" << time_init_point<< " (s)"<<std::endl;
    /*   std::size_t const os_threads = hpx::get_os_thread_count();
         hpx::naming::id_type const here = hpx::find_here();
         std::set<std::size_t> attendance;
         for(std::size_t os_thread =0; os_thread < os_threads; ++os_thread){
         attendance.insert(os_thread);
         }*/
    //    for (std::size_t i=0;i<max_levels;i++) {
 //     parents.push_back(std::vector<std::size_t>());
 //  }
    hpx::util::high_resolution_timer t_bfs_all;
    for(std::size_t i=0;i < searchroots.size();++i) {
      std::vector<hpx::lcos::future<std::vector<std::size_t> > > traverse_phase;
      bfs_localities::server::point::traverse_action traverse;
      std::size_t level = 0; 
      std::size_t parent = 9999; 
      // std::vector<std::vector<std::size_t> > parents;
      std::vector<std::size_t> parents;
      parents.reserve(vertex_num);
      for(std::size_t i =0;i<vertex_num;i++){
        parents[i] = 9999;
      }
      hpx::util::high_resolution_timer t_bfs;
      std::size_t root = searchroots[i];
      std::vector<std::size_t> visit_queue;
      visit_queue.reserve(vertex_num);
      std::vector<std::size_t> visited_queue;
      visited_queue.reserve(vertex_num);
      std::vector<std::vector<std::size_t> > neighbors;
      for(std::size_t i=0;i<vertex_num;i++) {
        neighbors.push_back(std::vector<std::size_t>());
      }

      parent = parents[root] =  root ; 
      //  traverse_phase.push_back( points[root].traverse_async(level,parent) );
      traverse_phase.push_back(hpx::async(traverse, points[root], level, parent) );
      // hpx::wait_all(traverse_phase);
      hpx::lcos::wait(traverse_phase, 
          [&](std::size_t, std::vector<std::size_t> t){
          neighbors.push_back(t);
          visit_queue.insert(visit_queue.end(), t.begin(), t.end());
          visited_queue.push_back(root);
          });

      while( visit_queue.size() != 0){
        traverse_phase.resize(0);
        for(std::size_t i =0;i<neighbors.size();i++){
          parent =  visited_queue.front(); 
          visited_queue.erase(visited_queue.begin());
          for(std::size_t j =0;j<neighbors[i].size(); j++){
            traverse_phase.push_back(hpx::async(traverse, points[neighbors[i][j]], 99, parent) );
          }
          hpx::lcos::wait(traverse_phase,
              [&](std::size_t, std::vector<std::size_t> t){
              neighbors.push_back(t);
              visit_queue.insert(visit_queue.end(), t.begin(), t.end());
              visited_queue.push_back(root);
              });
          //neighbors.erase(&(neighbors[i]));
        }
      }
      std::cout << "Elapsed time during "<<i<<"th bfs : " << t_bfs.elapsed()<< " (s)"<<std::endl;
    }
    std::cout << "Elapsed BFS all time: " << t_bfs_all.elapsed() << " [s]" << std::endl;
    std::cout << "Elapsed Execution all time: " << t_wall.elapsed() << " [s]" << std::endl;
  }

  return hpx::finalize(); // Initiate shutdown of the runtime system.
}


int main(int argc, char* argv[]){
  using boost::program_options::value;
  // Configure application-specific options.
  boost::program_options::options_description
    desc_commandline("Usage: " HPX_APPLICATION_STRING " [options]");

  desc_commandline.add_options()
    ("search_graph", value<std::string>()->default_value("root_search.txt"),
     "the search file containing the search root nodes")
    ("graph", value<std::string>()->default_value("graph.txt"),
     "the file containing the graph");

  return hpx::init(desc_commandline, argc, argv); 
}
