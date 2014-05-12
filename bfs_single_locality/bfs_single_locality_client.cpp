#include "bfs_single_locality_component.hpp"
#include <hpx/hpx.hpp>
#include <hpx/hpx_init.hpp>
#include <hpx/components/distributing_factory/distributing_factory.hpp>
#include <boost/ref.hpp>
#include <boost/format.hpp>
 #include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>
 #include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <hpx/lcos/wait_all.hpp>
#include <boost/foreach.hpp>
#include <time.h>
#include <hpx/include/lcos.hpp>

#include <fstream>
#include <iostream>

/*void matrix_to_adjlist(std::size_t *matrix, adjNode *adjList, int n){ 
  adjNode *tempNode;
  for(int i = 0;i<n;++i){//row n
    adjList[i].node = i;
    adjList[i].next = NULL;

    for(int j = n-1;j>=0;j--){//column n
      if(*(matrix+i*n+j) == 1){ 
        tempNode = (adjNode *) malloc(sizeof(adjNode));
        tempNode->next = adjList[i].next;
        tempNode->node = j;
        adjList[i].next = tempNode;
      }   
    }   
  }
}*/
/*inline void init(hpx::components::server::distributing_factory::iterator_range_type r,  std::vector<bfs_single_locality::point>& accu){
  BOOST_FOREACH(hpx::naming::id_type const& id, r)
    accu.push_back(bfs_single_locality::point(id));
}*/
inline std::size_t max_node(std::size_t n1, std::pair<std::size_t, std::size_t> const& p)
    {
        return (std::max)((std::max)(n1, p.first), p.second);
    }

inline bool read_edge_list(std::string const& graphfile,std::vector<std::pair<std::size_t, std::size_t> >& edgelist)
    {
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
inline bool read_search_node_list(std::string const& searchfile, std::vector<std::size_t>& searchroots)
    {
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

  hpx::util::high_resolution_timer t;
  {
    std::string const search_file = vm["search_graph"].as<std::string>();
    std::string const edge_file = vm["graph"].as<std::string>();

    typedef bfs_single_locality::server::graph graph_type;
    std::vector<hpx::id_type> localities = hpx::find_all_localities();
    bfs_single_locality::graph gra(hpx::components::new_<graph_type>(localities.back()));

    hpx::util::high_resolution_timer t_io;
    std::vector<std::pair<std::size_t, std::size_t> > edgelist;
    if (!read_edge_list(edge_file, edgelist))
      return -1;
     std::vector<std::size_t> searchroots;
     if (!read_search_node_list(search_file, searchroots))
       return -1;
     std::cout << "Elapsed time during read: " << t_io.elapsed()<< " (s)"<<std::endl;

    std::size_t vertex_num = std::accumulate(edgelist.begin(), edgelist.end(), std::size_t(0), max_node) +1;
    std::cout<<"vertex_num :"<<vertex_num<<std::endl;

    hpx::util::high_resolution_timer t_init;
    hpx::lcos::future<void> init_phase;
    init_phase = gra.init_async(0, edgelist, vertex_num);
    std::cout<<"Edge list :"<<std::endl;
    for(std::size_t i=0;i<edgelist.size();++i){
      std::cout<< edgelist[i].first<<" "<<edgelist[i].second<<std::endl;
    }
    std::cout<<"Search node:"<<std::endl;
    for(std::size_t i=0;i<searchroots.size();++i){
      std::cout<< searchroots[i]<<std::endl;
    }
   // hpx::lcos::wait(init_phase);
    std::cout << "Elapsed time during init: " << t_init.elapsed()<< " (s)"<<std::endl;

    std::vector<double> t_extime(searchroots.size(), 0.0);
    for(std::size_t i = 0; i< searchroots.size(); ++i){
      hpx::util::high_resolution_timer t_exe;
      std::vector<std::size_t> parents = gra.bfs_graph(searchroots[i]);
      t_extime[i] = t_exe.elapsed();
      std::cout << "Elapsed time during "<<i<<"th bfs : " << t_init.elapsed()<< " (s)"<<std::endl;
      for(std::size_t j =0;j < parents.size();++j){
        std::cout<< parents[j]<<" ";
      }
      std::cout<<std::endl;
    }
  }
  std::cout << "Elapsed total time: " << t.elapsed() << " (s)" << std::endl;
  return hpx::finalize(); // Initiate shutdown of the runtime system.
}


int main(int argc, char* argv[]){
  using boost::program_options::value;
  // Configure application-specific options.
  boost::program_options::options_description
    desc_commandline("Usage: " HPX_APPLICATION_STRING " [options]");

  desc_commandline.add_options()
    ("search_graph", value<std::string>()->default_value("g10_search.txt"),
     "the search file containing the search root nodes")
    ("graph", value<std::string>()->default_value("g10.txt"),
     "the file containing the graph");

  return hpx::init(desc_commandline, argc, argv); 
}
