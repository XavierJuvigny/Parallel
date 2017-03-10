// Copyright 2017 Dr. Xavier JUVIGNY

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
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
        m_impl->send(obj, dest, tag );
    }
    // .................................................................
    template<typename K> void 
    Communicator::send( std::size_t nbObjs, const K* buff, 
                        int dest, int tag ) const
    {
        m_impl->send(nbObjs, buff, dest, tag);
    }
    // .................................................................
    template<typename K> Request
    Communicator::isend( const K& obj, int dest, int tag ) const
    {
        return m_impl->isend( obj, dest, tag );
    }
    // .................................................................
    template<typename K> Request
    Communicator::isend( std::size_t nbItems, const K* obj, int dest, int tag ) const
    {
        return m_impl->isend( nbItems, obj, dest, tag );
    }    
    // .................................................................
    template<typename K> Status 
    Communicator::recv( K& obj, int sender, int tag ) const
    {
        return m_impl->recv( obj, sender, tag );
    }
    // .................................................................
    template<typename K> Status
    Communicator::recv( std::size_t nbObjs, K* buff, int sender, int tag ) const
    {
        return m_impl->recv( nbObjs, buff, sender, tag );
    }
    // .................................................................
    template<typename K> Request 
    Communicator::irecv( K& obj, int sender, int tag ) const
    {
        return m_impl->irecv( obj, sender, tag );
    }
    // .................................................................
    template<typename K> Request
    Communicator::irecv( std::size_t nbObjs, K* buff, int sender, int tag ) const
    {
        return m_impl->irecv( nbObjs, buff, sender, tag );
    }
    // =================================================================
    // Opérations collectives :
    template<typename K> void
    Communicator::bcast( const K& objsnd, K& objrcv, int root ) const
    {
      m_impl->broadcast( &objsnd, objrcv, root );
    }
    // .................................................................
    template<typename K> void
    Communicator::bcast( K& objrcv, int root ) const
    {
      m_impl->broadcast( static_cast<K*>(nullptr), objrcv, root );
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
    Communicator::reduce( const K& obj, K& res, const Operation& op, int root ) const
    {
        m_impl->reduce(obj, &res, op, root );
    }
    // .................................................................
    template<typename K> void
    Communicator::reduce( const K& obj, const Operation& op, int root ) const
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
    // _________________________________________________________________
    template<typename K> void
    Communicator::reduce( std::size_t nbItems, const K* obj, K* res,
                          Operation op, int root ) const
    {
        m_impl->reduce( nbItems, obj, res, op, root );
    }
    // .................................................................    
    template<typename K> void
    Communicator::reduce( std::size_t nbItems, const K* obj,
                          Operation op, int root ) const
    {
        assert(rank != root);
        m_impl->reduce( nbItems, obj, nullptr, op, root );
    }
    // _________________________________________________________________
    template<typename K, typename Func> void
    Communicator::reduce( std::size_t nbItems, const K* obj, K* res,
                          const Func& op, bool commute, int root ) const
    {
        m_impl->reduce( nbItems, obj, res, op, commute, root );
    }
    // .................................................................
    template<typename K, typename Func> void
    Communicator::reduce( std::size_t nbItems, const K* obj,
                          const Func& op, bool commute, int root ) const
    {
        assert(rank != root);
        m_impl->reduce( nbItems, obj, nullptr, op, commute, root );
    }
}
