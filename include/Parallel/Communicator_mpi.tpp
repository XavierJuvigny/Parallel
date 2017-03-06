// Communicator for MPI
#ifndef _PARALLEL_COMMUNICATOR_MPI_HPP_
#  define _PARALLEL_COMMUNICATOR_MPI_HPP_
# include <algorithm>
# include <cassert>
# include <mpi.h>
# include "Parallel/Status.hpp"
# include "Parallel/Constantes.hpp"
namespace Parallel
{
    struct Communicator::Implementation
    {
        Implementation()
        {
            MPI_Comm_dup( MPI_COMM_WORLD, &m_communicator );
        }
        // -------------------------------------------------------------
        Implementation( const Implementation& impl, int color, int key )
        {
            MPI_Comm_split( impl.m_communicator, color, key, 
                            &m_communicator );
        }
        // -------------------------------------------------------------
        Implementation( const Implementation& impl )
        {
            MPI_Comm_dup( impl.m_communicator, &m_communicator );
        }
        // -------------------------------------------------------------
        Implementation( const Ext_Communicator& excom )
        {
            MPI_Comm_dup( excom, &m_communicator );
        }
        // -------------------------------------------------------------
        ~Implementation() 
        {
            MPI_Comm_free(&m_communicator);
        }
        // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .        
        int getRank() const 
        { 
            int rank;
            MPI_Comm_rank(m_communicator, &rank);
            return rank; 
        }
        // -------------------------------------------------------------
        int getSize() const
        {
            int size;
            MPI_Comm_size(m_communicator, &size);
            return size;
        }
        // -------------------------------------------------------------
        Status probe( int src, int tag ) const
        {
            Status status;
            MPI_Probe( src, tag, m_communicator, &status.status );
            return status;
        }
        // -------------------------------------------------------------
        // Envoie par défaut :
        template<typename K> void send( std::size_t nbItems, const K* sndbuff,
                                        int dest, int tag ) const
        {
            if ( Type_MPI<K>::must_be_packed() ) {
                MPI_Send(sndbuff, nbItems*sizeof(K), MPI_BYTE,
                          dest, tag, m_communicator );
            } else {
                MPI_Send(sndbuff, nbItems, Type_MPI<K>::mpi_type(), 
                         dest, tag, m_communicator );
#               if defined(TRACE)
                std::cerr << "A envoyé buffer à l'adresse "
                          << (void*)sndbuff << " un message pour "
                          << dest << " avec le tag " << tag
                          << " contenant " << nbItems << " éléments"
                          << std::endl;
#               endif                
            }
        }
        // Réception par défaut :
        template<typename K> Status recv( std::size_t nbItems, K* rcvbuff,
                                        int sender, int tag ) const
        {
            Status status;
            if ( Type_MPI<K>::must_be_packed() ) {
                MPI_Recv(rcvbuff, nbItems*sizeof(K), MPI_BYTE,
                         sender, tag, m_communicator, &status.status);
            }
            else {
#               if defined(TRACE)
                std::cerr << "Reçoit dans buffer à l'adresse "
                          << (void*)rcvbuff << " un message provenant de "
                          << sender << " avec le tag " << tag
                          << " contenant " << nbItems << " éléments"
                          << std::endl;
#               endif
                MPI_Recv(rcvbuff, nbItems, Type_MPI<K>::mpi_type(),
                         sender, tag, m_communicator, &status.status );
#               if defined(TRACE)
                std::cerr << "OK, bien reçu !" << std::endl;
#               endif                         
            }
            return status;
        }
        // Broadcast :
        template<typename K> void 
        broadcast( std::size_t nbItems, const K* bufsnd, 
                   K* bufrcv, int root ) const
        {
            assert( bufrcv != nullptr );
            if (root == getRank()) {
                assert( bufsnd != nullptr );
                if ( bufsnd != bufrcv )
                    std::copy_n( bufsnd, nbItems, bufrcv );
            }
            if ( Type_MPI<K>::must_be_packed() ) {
                MPI_Bcast(bufrcv, nbItems*sizeof(K), MPI_BYTE, 
                          root, m_communicator );
            } else {
                MPI_Bcast(bufrcv, nbItems, Type_MPI<K>::mpi_type(),
                          root, m_communicator );
            }
        }
        template<typename K> Request
        ibroadcast( std::size_t nbItems, const K* bufsnd, 
                    K* bufrcv, int root ) const
        {
            assert( bufrcv != nullptr );
            if (root == getRank()) {
                assert( bufsnd != nullptr );
                if ( bufsnd != bufrcv )
                    std::copy_n( bufsnd, nbItems, bufrcv );
            }
            Request req;
            MPI_Ibcast( bufrcv, nbItems, Type_MPI<K>::mpi_type(),
                        root, m_communicator, &req );
            return req;
        }
    private:
        MPI_Comm m_communicator;
    };
    // -----------------------------------------------------------------
    
}


#endif
