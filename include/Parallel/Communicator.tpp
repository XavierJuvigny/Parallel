// template for Communicator class
# include <iostream>
# if defined(USE_MPI)
#   include "Parallel/Communicator_mpi.tpp"
# else
#   include "Parallel/Communicator_stub.tpp"
# endif

namespace Parallel
{
    template<typename K> void 
    Communicator::send( const K& obj, int dest, int tag ) const
    {
        m_impl->send(1, &obj, dest, tag );
    }
    // .................................................................
    template<typename K> void 
    Communicator::send( std::size_t nbObjs, const K* buff, 
                        int dest, int tag ) const
    {
#       if defined(TRACE)
        std::cerr << "Envoie tableau d'objets de taille " << nbObjs*sizeof(K) 
                  << "octets au processus n°"
                  << dest << " avec le tag " << tag << std::endl;
#       endif
        m_impl->send(nbObjs, buff, dest, tag);
    }
    // .................................................................
    template<typename K> void 
    Communicator::send( const std::vector<K>& arr, int dest, int tag ) const
    {
#       if defined(TRACE)
        std::cerr << "Envoie vecteur de taille " << arr.size() << " au processus n°"
                  << dest << " avec le tag " << tag << std::endl;
#       endif
        m_impl->send(arr.size(), arr.data(), dest, tag);
    }
    // .................................................................
    template<typename K> Request
    Communicator::isend( const K& obj, int dest, int tag ) const
    {
#       if defined(TRACE)
        std::cerr << "Non blocking send of an object of size  " << sizeof(K)
                  << " bytes to process " << dest << " with tag " << tag
                  << std::endl;
#       endif
        return m_impl->isend( 1, &obj, dest, tag );
    }
    // .................................................................
    template<typename K> Request
    Communicator::isend( const std::vector<K>& obj, int dest, int tag ) const
    {
#       if defined(TRACE)
        std::cerr << "Non blocking send of a vector of objects of size  " << obj.size()
                  << " items to process " << dest << " with tag " << tag
                  << std::endl;
#       endif
        return m_impl->isend( obj.size(), obj.data(), dest, tag );
    }
    // .................................................................
    template<typename K> Request
    Communicator::isend( std::size_t nbItems, const K* obj, int dest, int tag ) const
    {
#       if defined(TRACE)
        std::cerr << "Non blocking send of a buffer of objects of size  " << nbItems*sizeof(K)
                  << " bytes to process " << dest << " with tag " << tag
                  << std::endl;
#       endif
        return m_impl->isend( nbItems, obj, dest, tag );
    }    
    // .................................................................
    template<typename K> Status 
    Communicator::recv( K& obj, int sender, int tag ) const
    {
        // Cas particuliers où K est un std::vector
        /*if ( std::is_same<K,std::vector<typename K::value_type,
                                        typename K::allocator_type>>::value ) */
        return m_impl->recv( 1, &obj, sender, tag );
    }
    // .................................................................
    template<typename K> Status
    Communicator::recv( std::size_t nbObjs, K* buff, int sender, int tag ) const
    {
        return m_impl->recv( nbObjs, buff, sender, tag );
    }
    // .................................................................
    template<typename K> Status
    Communicator::recv( std::vector<K>& array, int sender, int tag ) const
    {
#       if defined(TRACE)
        std::cerr << "Reçoit un vecteur du processus " << sender 
                  << " avec le tag " << tag << "...";
#       endif
        Status status = m_impl->probe(sender, tag);
#       if defined(TRACE)
        std::cerr << " reçoit message de taille " 
                  << status.count<K>()
                  << std::endl;
#       endif
        std::vector<K>(status.count<K>()).swap(array);
        return m_impl->recv( array.size(), array.data(), sender, tag );
    }
    // =================================================================
    // Opérations collectives :
    template<typename K> void
    Communicator::bcast( const K& objsnd, K& objrcv, int root ) const
    {
        m_impl->broadcast( 1, &objsnd, &objrcv, root );
    }
    // .................................................................
    template<typename K> void
    Communicator::bcast( K& objrcv, int root ) const
    {
        m_impl->broadcast( 1, nullptr, &objrcv, root );
    }
    // .................................................................
    template<typename K> void
    Communicator::bcast( const std::vector<K>& u_snd, std::vector<K>& u_rcv,
                         int root ) const
    {
        if ( u_rcv.size() < u_snd.size() )
            std::vector<K>(u_snd.size()).swap(u_rcv);
        m_impl->broadcast(u_snd.size(),u_snd.data(),u_rcv.data(),root);
    }
    // .................................................................
    template<typename K> void
    Communicator::bcast( std::vector<K>& u_rcv, int root ) const
    {
        m_impl->broadcast(u_rcv.size(), (const K*)nullptr,u_rcv.data(),root);
    }
    // .................................................................
    template<typename K> void 
    Communicator::bcast( std::size_t nbObjs, const K* b_snd, K* b_rcv, int root ) const
    {
        m_impl->broadcast(nbObjs, b_snd, b_rcv, root);
    }
    // .................................................................
    template<typename K> void 
    Communicator::bcast( std::size_t nbObjs, K* b_rcv, int root ) const
    {
        m_impl->broadcast(nbObjs, (const K*)nullptr, b_rcv, root);
    }
    // =================================================================
    template<typename K> void
    Communicator::reduce( const K& obj, K& res, Operation op, int root ) const
    {
        m_impl->reduce(1, &obj, &res, op, root );
    }
    // .................................................................
    template<typename K> void
    Communicator::reduce( const K& obj, Operation op, int root ) const
    {
        assert(root != rank);
        m_impl->reduce(1, &obj, nullptr, op, root );
    }
    // _________________________________________________________________
    template<typename K, typename Func> void
    Communicator::reduce( const K& obj, K& res, const Func& op, 
                          bool commute, int root ) const
    {
        m_impl->reduce(1, &obj, &res, op, commute, root );
    }
    // .................................................................
    template<typename K, typename Func> void
    Communicator::reduce( const K& obj, const Func& op, bool commute, 
                          int root ) const
    {
        assert(root != rank);
        m_impl->reduce(1, &obj, nullptr, op, commute, root );
    }
    // _________________________________________________________________
    template<typename K> void
    Communicator::reduce( const std::vector<K>& obj, std::vector<K>& res,
                          Operation op, int root ) const
    {
        if ( res.size() < obj.data() ) {
            std::vector<K>(obj.size()).swap(res);
        }
        m_impl->reduce(obj.size(), obj.data(), res.data(), op, root );        
    }
    // .................................................................
    template<typename K> void
    Communicator::reduce( const std::vector<K>& obj, Operation op, int root ) const
    {
        assert(root != rank );
        m_impl->reduce(obj.size(), obj.data(), nullptr, op, root );
    }
    // _________________________________________________________________
    template<typename K, typename Func> void
    Communicator::reduce( const std::vector<K>& obj, std::vector<K>& res,
                          const Func& op, bool commute, int root) const
    {
        if ( res.size() < obj.data() ) {
            std::vector<K>(obj.size()).swap(res);
        }
        m_impl->reduce(obj.size(), obj.data(), res.data(), op, 
                       commute, root );
    }
    // .................................................................
    template<typename K, typename Func> void
    Communicator::reduce( const std::vector<K>& obj, const Func& op,
                          bool commute, int root ) const
    {
        assert(root != rank);
        m_impl->reduce(obj.size(), obj.data(), nullptr, op, 
                       commute, root );
    }
}
