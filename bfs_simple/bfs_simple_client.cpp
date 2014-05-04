#include "bfs_component.hpp"
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
inline void init(hpx::components::server::distributing_factory::iterator_range_type r,  std::vector<bfs_simple::point>& accu){
  BOOST_FOREACH(hpx::naming::id_type const& id, r)
    accu.push_back(bfs_simple::point(id));
}

int hpx_main(boost::program_options::variables_map& vm){
  {
    hpx::util::high_resolution_timer t;
    std::size_t const num = vm["n"].as<std::size_t>();
    std::size_t const root = vm["root"].as<std::size_t>();
    std::size_t const max_levels = vm["max-levels"].as<std::size_t>();
    std::size_t const max_num_neighbors
      = vm["max-num-neighbors"].as<std::size_t>();
    std::string const edge_file = vm["graph"].as<std::string>();

    //   std::size_t nodes[num]={0};
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
    hpx::components::distributing_factory factory;
    factory.create(hpx::applier::get_applier().get_runtime_support_gid());  
    //factory.create(hpx::find_here());
  //  std::vector<hpx::id_type> localities = hpx::find_all_localities();   


    hpx::components::component_type block_type = 
     hpx::components::get_component_type<bfs_simple::server::point>();

    hpx::components::distributing_factory::result_type blocks =
      factory.create_components(block_type,  num);

    std::vector<bfs_simple::point> points;
    //BOOST_FOREACH(hpx::naming::id_type const& id, localities)
    ::init(hpx::util::locality_results(blocks), points);
    
    for(std::size_t i = 0; i< num; i++)
    {
     // points.push_back(bfs_simple::point(hpx::components::new_<std::size_t>id, i, neighbors[i]));
      points[i].init(i, node_neighbors[i]);
    }

    std::vector<hpx::lcos::promise<std::vector<std::size_t> > > traverse_phase;
    std::size_t level = 0; 
    std::size_t parent = 9999; 
    std::vector<std::vector<std::size_t> > parents;
    for (std::size_t i=0;i<max_levels;i++) {
      parents.push_back(std::vector<std::size_t>());
    }
    std::vector<std::vector<std::size_t> > neighbors,alt_neighbors;
    parents[level].push_back( root ); 
    traverse_phase.push_back( points[root].traverse_async(level,parent) );

    hpx::lcos::wait(traverse_phase,neighbors);

    for (std::size_t k=1;k<max_levels;k++) {
      traverse_phase.resize(0);

      if ( (k+1)%2 == 0 ) {
        alt_neighbors.resize(0);
        for (std::size_t i=0;i<neighbors.size();i++) {
          parent = parents[k-1][i];
          for (std::size_t j=0;j<neighbors[i].size();j++) {
            parents[k].push_back( neighbors[i][j] ); 
            traverse_phase.push_back(points[ neighbors[i][j] ].traverse_async(k,parent));
          } 
        }
        hpx::lcos::wait(traverse_phase,alt_neighbors);
        //hpx::wait_all(traverse_phase);
      } else {
        neighbors.resize(0);
        for (std::size_t i=0;i<alt_neighbors.size();i++) {
          parent = parents[k-1][i];
          for (std::size_t j=0;j<alt_neighbors[i].size();j++) {
            parents[k].push_back( alt_neighbors[i][j] ); 
            traverse_phase.push_back(points[ alt_neighbors[i][j] ].traverse_async(k,parent));
          } 
        }
     /*   std::vector<hpx::lcos::future<void> > barrier;
        for (std::size_t i=0;i<iterations;i++) {
          std::size_t rn = rand() % array_size;
          barrier.push_back(accu[rn].add_async()); 
        }
        hpx::wait_all(barrier);  
        */
       hpx::lcos::wait(traverse_phase,neighbors);
       // hpx::wait_all(traverse_phase);
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
    ("n", value<std::size_t>()->default_value(100),
     "the number of nodes in the graph")
    ("max-num-neighbors", value<std::size_t>()->default_value(20),
     "the maximum number of neighbors")
    ("max-levels", value<std::size_t>()->default_value(20),
     "the maximum number of levels to traverse")
    ("root", value<std::size_t>()->default_value(0),
     "the root node in the graph")
    ("graph", value<std::string>()->default_value("test.txt"),
     "the file containing the graph");

  return hpx::init(desc_commandline, argc, argv); 
}
