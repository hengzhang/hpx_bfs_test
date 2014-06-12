#include "bfs_simple_component.hpp"
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

inline void init(hpx::components::server::distributing_factory::iterator_range_type r,  std::vector<hpx::naming::id_type>& accu){
  BOOST_FOREACH(hpx::naming::id_type const& id, r)
    accu.push_back(id);
}

int hpx_main(boost::program_options::variables_map& vm){
  {
    hpx::util::high_resolution_timer t;
    std::size_t const num = vm["n"].as<std::size_t>();
    std::size_t const root = vm["root"].as<std::size_t>();
    std::size_t const max_levels = vm["max-levels"].as<std::size_t>();
    std::string const edge_file = vm["graph"].as<std::string>();

    std::vector<std::vector<std::size_t> >  node_neighbors(num);

    std::string line;
    std::string val1,val2;
    std::ifstream myfile;
    myfile.open(edge_file);
    if (myfile.is_open()) {
      while (myfile.good()) { 
        while (std::getline(myfile,line)) {
          std::istringstream isstream(line);
          std::getline(isstream,val1,' ');
          std::getline(isstream,val2,' ');
          std::size_t node = boost::lexical_cast<std::size_t>(val1);   
          std::size_t neighbor = boost::lexical_cast<std::size_t>(val2);   
          //construct n -> neighbors
          node_neighbors[node].push_back(neighbor); 
        }
      }
    }
    for(std::size_t i =0 ;i< num; i++){
      std::cout<<"node "<<i<<":";
      for(std::vector<std::size_t>::iterator it = node_neighbors[i].begin();it != node_neighbors[i].end();++it)
        std::cout<<" "<<*it;
      std::cout<<'\n';
    }

    hpx::components::distributing_factory factory =  hpx::components::distributing_factory::create(hpx::find_here());
    std::vector<hpx::id_type> localities = hpx::find_all_localities();   

    hpx::components::component_type block_type = 
      bfs_simple::server::point::get_component_type();

    hpx::components::distributing_factory::result_type blocks =
      factory.create_components(block_type,  num);

    std::vector<hpx::naming::id_type> points;
    ::init(hpx::util::locality_results(blocks), points);
    std::vector<hpx::lcos::future<void> > init_phase;
    bfs_simple::server::point::init_action init;
    for(std::size_t i = 0; i< num; i++)
    {
      init_phase.push_back(hpx::async(init, points[i], i, node_neighbors[i]));
    }
    hpx::wait_all(init_phase);

    std::size_t level = 0; 
    std::size_t parent = 9999; 
    std::vector<std::vector<std::size_t> > parents;
    std::vector<hpx::lcos::future<std::vector<std::size_t> > > traverse_phase;
    bfs_simple::server::point::traverse_action traverse;
    for (std::size_t i=0;i<max_levels;i++) {
      parents.push_back(std::vector<std::size_t>());
    }
    std::vector<std::vector<std::size_t> > neighbors,alt_neighbors;
    parents[level].push_back( root ); 
    traverse_phase.push_back(hpx::async(traverse, points[root], level, parent));

    //hpx::lcos::wait(traverse_phase,neighbors);
    hpx::lcos::wait(traverse_phase, 
        [&](std::size_t, std::vector<std::size_t> t){
          neighbors.push_back(t);
        });
//    std::cout<<"neighbors:neighbors.size,"<<neighbors.size()<<std::endl;

    for (std::size_t k=1;k<max_levels;k++) {
      traverse_phase.resize(0);
      if ( (k+1)%2 == 0 ) {
        alt_neighbors.resize(0);
        for (std::size_t i=0;i<neighbors.size();i++) {
          parent = parents[k-1][i];
          for (std::size_t j=0;j<neighbors[i].size();j++) {
            parents[k].push_back( neighbors[i][j] ); 
            traverse_phase.push_back(hpx::async(traverse, points[root], level, parent));
          } 
        }
        //hpx::lcos::wait(traverse_phase,alt_neighbors);
        hpx::lcos::wait(traverse_phase, 
            [&](std::size_t, std::vector<std::size_t> t){
              alt_neighbors.push_back(t);
        });
      } else {
        neighbors.resize(0);
        for (std::size_t i=0;i<alt_neighbors.size();i++) {
          parent = parents[k-1][i];
          for (std::size_t j=0;j<alt_neighbors[i].size();j++) {
            parents[k].push_back( alt_neighbors[i][j] ); 
            traverse_phase.push_back(hpx::async(traverse, points[root], level, parent));
          } 
        }
        hpx::lcos::wait(traverse_phase, 
            [&](std::size_t, std::vector<std::size_t> t){
              neighbors.push_back(t);
        });
      }
    }
    std::cout << "Elapsed time: " << t.elapsed() << " [s]" << std::endl;
   }

  return hpx::finalize(); // Initiate shutdown of the runtime system.
}

int main(int argc, char* argv[]){
  using boost::program_options::value;
  // Configure application-specific options.
  boost::program_options::options_description
    desc_commandline("Usage: " HPX_APPLICATION_STRING " [options]");

  desc_commandline.add_options()
    ("n", value<std::size_t>()->default_value(15),
     "the number of nodes in the graph")
    ("max-levels", value<std::size_t>()->default_value(20),
     "the maximum number of levels to traverse")
    ("root", value<std::size_t>()->default_value(0),
     "the root node in the graph")
    ("graph", value<std::string>()->default_value("test.txt"),
     "the file containing the graph");

  return hpx::init(desc_commandline, argc, argv); 
}
