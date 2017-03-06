// Implementation of the Communicator class
# include "Parallel/Communicator.hpp"
# if defined(MPI_VERSION)
#   include "Parallel/Communicator_mpi.tpp"
# else
#   include "Parallel/Communicator_stub.tpp"
# endif
namespace Parallel
{
    Communicator::Communicator() : 
        m_impl( new Communicator::Implementation)
    {
        rank = m_impl->getRank();
        size = m_impl->getSize();
    }
    // .................................................................
    Communicator::Communicator( const Communicator& com, 
                                int color, int key ) :
        m_impl(new Communicator::Implementation(*com.m_impl,color,key))
    {
        rank = m_impl->getRank();
        size = m_impl->getSize();        
    }
    // .................................................................
    Communicator::Communicator( const Communicator& com ) :
        m_impl(new Communicator::Implementation(*com.m_impl))
    {
        rank = m_impl->getRank();
        size = m_impl->getSize();        
    }
    // .................................................................
    Communicator::~Communicator()
    {
        delete m_impl;
    }
    // =================================================================
}
