// Status des réceptions/envois non synchros
# ifndef _PARALLEL_STATUS_HPP_
# define _PARALLEL_STATUS_HPP_
# ifdef USE_MPI
# include <mpi.h>
# endif
# include "Parallel/Constantes.hpp"
namespace Parallel
{
# if defined(USE_MPI)
    struct Status
    {
    public:
      Status() = default;
        Status( MPI_Status st ) : status(st)
        {}
        MPI_Status status;
        template<typename K> int count() const { 
            int cnt; MPI_Get_count(&status, Type_MPI<K>::mpi_type(), &cnt);
            return cnt;
        }
        int source() const { return status.MPI_SOURCE; }
        int tag() const { return status.MPI_TAG; }
        int error() const { return status.MPI_ERROR; }
    };
# else
    /*!
     *    \struct Status
     *    \brief  The state of an incoming message
     *    
     */
    struct Status
    {
        /*!
         *    \brief Return the number of objects contained in the incoming message
         */
        template<typename K> int count() const { return m_count; }
        /*!
         *    \brief Return the identity tag of the incoming message
         */
        int tag  () const { return m_tag  ; }
        /*!
         *    \brief Return the rank of the sender of the incoming message
         */
        int source   () const { return 0; }
        /*!
         *    \brief Return the error state of the incoming message.
         */
        int error    () const { return m_error; }
        /// \privatesection
        int m_count, m_tag,m_error;
    };
# endif
}
#endif
