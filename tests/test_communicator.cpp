// Test des communications intra--communicateur
# include <iostream>
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
    context.pout << "Tableau final :" << std::endl;
    for ( auto& val : array ) {
        context.pout << val << " ";
        std::cout << val << " ";
    }
    context.pout << std::endl;
    std::cout << std::endl;
    return EXIT_SUCCESS;
}
