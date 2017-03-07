// Stub for parallel library ( to work on standalone computer by example )
# include <algorithm>
# include <iostream>
# include "Parallel/Status.hpp"
# include "Parallel/Constantes.hpp"
namespace Parallel
{
    struct Communicator::Implementation
    {
        Implementation() : m_pt_sendbuffer(nullptr)
        {}
        // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .        
        Implementation( const Implementation& impl, int color, int key ) :
                            m_pt_sendbuffer(nullptr)
        {}
        // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .        
        Implementation( const Implementation& impl ) :
                            m_pt_sendbuffer(nullptr)
        {}
        // .............................................................
        Implementation( const Ext_Communicator& com ) :
                            m_pt_sendbuffer(nullptr)
        {}
        // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .        
        ~Implementation() {}
        // .............................................................
        int getRank() const { return 0; }
        // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .        
        int getSize() const { return 1; }
        // .............................................................
        template<typename K> error send( std::size_t nbItems, const K* sndbuff,
                                         int dest, int tag ) const
        {
            if (dest != 0) return error::rank;
            if ( sndbuff == nullptr ) return error::buffer;
            m_nbItems = nbItems;
            m_pt_sendbuffer = new K[nbItems];
            std::copy_n(sndbuff, nbItems, (K*)(m_pt_sendbuffer));
            return error::success;
        }
        // .............................................................
        template<typename K> Status recv( std::size_t nbItems, K* rcvbuff,
                                        int sender, int tag ) const
        {
            if ( m_pt_sendbuffer == nullptr ) {
                return Status{.m_count = 0, .m_tag = 0, .m_error = error::buffer};
            }
            Status stat;
            if ( m_nbItems < nbItems ) {
                stat = Status{.m_count = int(m_nbItems), .m_tag = 0, .m_error = error::count};
                nbItems = m_nbItems;
            }
            else 
                stat = Status{.m_count = int(nbItems), .m_tag = tag, .m_error = error::success};
            if ( m_pt_sendbuffer != rcvbuff )
                std::copy_n( static_cast<const K*>(m_pt_sendbuffer), 
                             nbItems, rcvbuff );
            delete [] (K*)m_pt_sendbuffer;
            m_pt_sendbuffer = nullptr;
            return stat;
        }
        // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .        
        Status probe( int src, int tag ) const
        {
            return Status{.m_count = int(m_nbItems), .m_tag = tag, .m_error = error::success};
        }
        // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .        
        template<typename K> void broadcast( std::size_t nbItems, 
                                             const K* bufsnd, K* bufrcv,
                                             int root ) const
        {
            if ( bufsnd != bufrcv )
                std::copy_n( bufsnd, nbItems, bufrcv );
        }
        // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .        
        template<typename K> Request ibroadcast( std::size_t nbItems, 
                                                 const K* bufsnd, K* bufrcv,
                                                 int root ) const
        {
            if ( bufsnd != bufrcv )
                std::copy_n( bufsnd, nbItems, bufrcv     );
            return Request(1);
        }
        // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
        void barrier() const {}        
        // .............................................................
    private:
        mutable std::size_t m_nbItems;
        mutable const void* m_pt_sendbuffer;        
    };
    // -----------------------------------------------------------------
    
}
