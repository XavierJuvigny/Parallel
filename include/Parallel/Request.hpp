// Request
#ifndef _PARALLEL_REQUEST_HPP_
# define _PARALLEL_REQUEST_HPP_ 
# include "Parallel/Constantes.hpp"

# ifdef USE_MPI
# include <mpi.h>
namespace Parallel
{
    class Request
    {
    public:
        Request( const MPI_Request& req ) : m_req(req)
        {}
        bool test() {
            int flag;
            MPI_Test( &m_req, &flag, &m_status );
            return (flag!=0);
        }
        void wait() {
            MPI_Wait( &m_req, &m_status );
        }
        Status status() const { return Status(m_status); }
    private:
        MPI_Request m_req;
        MPI_Status  m_status;
    };
    // Waitall to do, not so easy !
}
# else
    class Request
    {
    public:
        Request() {}
        bool test() { return true; }
        void wait() {}
        // To think about status... Some trick to do ?
        Status status() const { return Status{.m_count=0, .m_tag = 0, .m_error = 0}; }
    };
// TO DO
# endif
#endif
