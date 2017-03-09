// Test des communications intra--communicateur
# include <iostream>
# include <cmath>
# include "Parallel/Parallel.hpp"
# include "Parallel/LogToFile.hpp"

int main( int nargs, char* argv[] )
{
    Parallel::Context context(nargs, argv);
    Parallel::Logger& log = Parallel::Context::logger;
    int listeners = Parallel::Logger::Listener::Listen_for_assertion +
                    Parallel::Logger::Listener::Listen_for_error +
                    Parallel::Logger::Listener::Listen_for_warning +
                    Parallel::Logger::Listener::Listen_for_information;
    log.subscribe(new Parallel::LogToFile("Output",listeners));
    if ( nargs > 1 ) {
      if ( std::string(argv[1]) == std::string("trace") )
        log.subscribe(new Parallel::LogToFile("Trace",Parallel::Logger::Listener::Listen_for_trace));
    }
      
    // Par défaut, communicateur global :
    Parallel::Communicator com;
    std::vector<int> array;
    if ( com.rank == 0 ) {
        std::vector<int>(com.size,0).swap(array);
        array[0] = 1;
        com.send(array, 1%com.size);
        com.recv(array, com.size-1);
        // Test Broadcast ( root = 0 par défaut ) :
        com.bcast(array,array);
    } else {
        com.recv(array, com.rank-1);
        array[com.rank] = 1;
        com.send(array, (com.rank+1)%com.size);
        com.bcast(array);
    }
    com.barrier();
    log << "Final array : ";
    for ( auto& val : array ) {
      log << val << " ";
    }
    log << "\n";
    com.barrier();
    double x = (com.rank+1)*1.5;
    double y;
    com.reduce(x,y, [](const double& x, const double& y) -> double { return sin(x)+sin(y); }, true, 0);
    
    if ( com.rank == 0 )
      log << "Reduction : " << y << "\n";

    Parallel::Request rreq = com.irecv(array, (com.rank+com.size-1)%com.size );
    std::vector<int> tab(com.size,0);
    tab[com.rank] = 1;
    Parallel::Request sreq = com.isend(tab, (com.rank+1)%com.size );
    sreq.wait();
    rreq.wait();
    log << "isend result array : ";
    for ( const auto& t : array )
      log << t << " ";
    log << "\n";
    return EXIT_SUCCESS;
}
