// Test des communications intra--communicateur
# include <iostream>
# include <cmath>
# include "Parallel/Parallel.hpp"

int main( int nargs, char* argv[] )
{
    Parallel::Context context(nargs, argv);
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
    std::cerr << "Tableau final :" << std::endl;
    com.barrier();
    for ( auto& val : array ) {
        context.pout << val << " ";
    }
    context.pout << std::endl;
    com.barrier();
    double x = (com.rank+1)*1.5;
    double y;
    
    com.reduce(x,y, [](const double& x, const double& y) -> double { return sin(x)+sin(y); }, true, 0);
    
    if ( com.rank == 0 )
        context.pout << "Réduction : " << y << std::endl;
    std::cerr << com.rank << " : Fin écriture" << std::endl;

    std::vector<int> tab(com.size,0);
    tab[com.rank] = 1;
    Parallel::Request req = com.isend(tab, (com.rank+1)%com.size );
    req.wait();
    com.recv(array, (com.rank+com.size-1)%com.size );
    for ( const auto& t : array ) context.pout << t << " ";
    context.pout << std::endl;
    return EXIT_SUCCESS;
}
