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
// Communicator for MPI
#ifndef _PARALLEL_COMMUNICATOR_MPI_HPP_
#  define _PARALLEL_COMMUNICATOR_MPI_HPP_
# include <algorithm>
# include <functional>
# include <cassert>
# include <iostream>
# include <mpi.h>
# include "Parallel/Status.hpp"
# include "Parallel/Constantes.hpp"
# include "Parallel/DetectContainer.hpp"
# include "Parallel/Logger.hpp"
# include "Parallel/Context.hpp"

namespace Parallel
{
    namespace {
    template<typename K> std::function<K(const K&, const K&)> reduce_functor;
    template<typename K> void reduce_user_function ( void* x, void* y, int* length, 
                                                     MPI_Datatype* tp )
    {
        K val;
        K* ax = (K*)x;
        K* ay = (K*)y;
        for ( int i = 0; i < *length; ++i) {
            val = reduce_functor<K>(ax[i], ay[i]);
            ay[i] = val;
        }
    }
    }
    // .................................................................
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

        template<typename K, bool flag = false>
        struct Communication
        {
          static void send( const MPI_Comm& com, const K& snd_obj, int dest, int tag )
          {
            if ( Type_MPI<K>::must_be_packed() ) {
              MPI_Send(&snd_obj, sizeof(K), MPI_BYTE, dest, tag, com );
            } else {
              MPI_Send(&snd_obj, 1, Type_MPI<K>::mpi_type(), dest, tag, com );
            }
#           if defined(DEBUG)
            LogTrace << "Send an object to " << dest << " with tag " << tag << "\n";
#           endif
          }
          // .......................................................................................
          static Request isend( const MPI_Comm& com, const K& snd_obj, int dest, int tag )
          {
            MPI_Request m_req;
            if ( Type_MPI<K>::must_be_packed() ) {
              MPI_Isend(&snd_obj, sizeof(K), MPI_BYTE, dest, tag, com,  &m_req);
            } else {
              MPI_Isend(&snd_obj, 1, Type_MPI<K>::mpi_type(), dest, tag, com, &m_req );
            }
#           if defined(DEBUG)
            LogTrace << "Asynchrone send for an object to " << dest << " with tag "
                     << tag << "\n";
#           endif
            return Request(m_req);            
          }
          // .......................................................................................
          static Status recv( const MPI_Comm& com, K& rcvobj, int sender, int tag )
          {
#           if defined(DEBUG)            
            LogTrace << "Receive an object from " << sender << " with tag "
                     << tag << "\n";
#           endif
            Status status;
            if ( Type_MPI<K>::must_be_packed() ) {
              MPI_Recv(&rcvobj, sizeof(K), MPI_BYTE, sender, tag, com, &status.status);
            }
            else {
              MPI_Recv(&rcvobj, 1, Type_MPI<K>::mpi_type(), sender, tag, com, &status.status );
            }
#           if defined(DEBUG)            
            LogTrace << "OK, receive done !" << "\n";
#           endif
            return status;
          }
          // .......................................................................................
          static Request irecv( const MPI_Comm& com, K& rcvobj, int sender, int tag )
          {
#           if defined(DEBUG)            
            LogTrace << "Asynchronous receive of an object from " << sender << " with tag "
                     << tag << "\n";
#           endif
            MPI_Request req;
            if ( Type_MPI<K>::must_be_packed() ) {
              MPI_Irecv(&rcvobj, sizeof(K), MPI_BYTE, sender, tag, com, &req);
            } else {
              MPI_Irecv(&rcvobj, 1, Type_MPI<K>::mpi_type(), sender, tag, com, &req );
            }
            return Request(req);
          }
          // .......................................................................................
          static void broadcast( const MPI_Comm& com, const K* obj_snd, K& obj_rcv, int root )
          {
#           if defined(DEBUG)            
            LogTrace << "Broadcast of one object with root = " << root << "\n";
#           endif
            int rank;
            MPI_Comm_rank(com, &rank);
            if (root == rank) {
              assert(obj_snd != nullptr);
              obj_rcv = *obj_snd;
            }
            if ( Type_MPI<K>::must_be_packed() ) {
              MPI_Bcast(&obj_rcv, sizeof(K), MPI_BYTE, root, com );
            } else {
              MPI_Bcast(&obj_rcv, 1, Type_MPI<K>::mpi_type(), root, com );
            }
          }
          // .......................................................................................
          static void reduce( const MPI_Comm& com, const K& loc, K* glob,
                              const Operation& op, int root ) {
#           if defined(DEBUG)            
            LogTrace << "Reduce operation on one object with root = " << root
                     << " and store at adress " << (void*)glob << "\n";
            int rank;
            MPI_Comm_rank(com, &rank);
            assert( (rank!=root) || (glob != nullptr) );
#           endif
            MPI_Reduce( &loc, glob, 1, Type_MPI<K>::mpi_type(), op, root, com );
#           if defined(DEBUG)            
            LogTrace << "End of reduction" << "\n";
#           endif
          }
        };      
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
                    << "\n";
#               endif                
        }
      }
      // -------------------------------------------------------------------------------------------
      template<typename K> void send( const K& snd, int dest, int tag ) const
      {
        Communication<K,is_container<K>::value>::send(m_communicator, snd,dest,tag);
      }
      //
      template<typename K> Request isend( std::size_t nbItems, const K* sndbuff,
                                          int dest, int tag ) const
      {
        MPI_Request m_req;
        if ( Type_MPI<K>::must_be_packed() ) {
          MPI_Isend(sndbuff, nbItems*sizeof(K), MPI_BYTE,
                    dest, tag, m_communicator,  &m_req);
        } else {
          MPI_Isend(sndbuff, nbItems, Type_MPI<K>::mpi_type(), 
                    dest, tag, m_communicator, &m_req );
#         if defined(TRACE)
          std::cerr << "A envoyé buffer à l'adresse "
                    << (void*)sndbuff << " un message pour "
                    << dest << " avec le tag " << tag
                    << " contenant " << nbItems << " éléments"
                    << "\n";
#         endif                
        }
        return Request(m_req);
      }
      template<typename K> Request isend( const K& snd, int dest, int tag ) const
      {
        return Communication<K,is_container<K>::value>::isend(m_communicator, snd,dest,tag);
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
                          << "\n";
#               endif
                MPI_Recv(rcvbuff, nbItems, Type_MPI<K>::mpi_type(),
                         sender, tag, m_communicator, &status.status );
#               if defined(TRACE)
                std::cerr << "OK, bien reçu !" << "\n";
#               endif                         
            }
            return status;
        }
      template<typename K> Status recv( K& rcvobj, int sender, int tag ) const
      {
        return Communication<K,is_container<K>::value>::recv(m_communicator,
                                                             rcvobj, sender, tag);
      }
        template<typename K> Request irecv( std::size_t nbItems, K* rcvbuff,
                                            int sender, int tag ) const
        {
            MPI_Request req;
            if ( Type_MPI<K>::must_be_packed() ) {
                MPI_Irecv(rcvbuff, nbItems*sizeof(K), MPI_BYTE,
                         sender, tag, m_communicator, &req);
            }
            else {
#               if defined(TRACE)
                std::cerr << "Reçoit dans buffer à l'adresse "
                          << (void*)rcvbuff << " un message non bloquant provenant de "
                          << sender << " avec le tag " << tag
                          << " contenant " << nbItems << " éléments"
                          << "\n";
#               endif
                MPI_Irecv(rcvbuff, nbItems, Type_MPI<K>::mpi_type(),
                         sender, tag, m_communicator, &req );
            }
            return Request(req);
        }
      template<typename K> Request irecv( K& rcvobj, int sender, int tag ) const
      {
        return Communication<K,is_container<K>::value>::irecv(m_communicator,
                                                              rcvobj, sender, tag);
      }

      // Broadcast :
      template<typename K> void 
      broadcast( std::size_t nbItems, const K* bufsnd, 
                 K* bufrcv, int root ) const
      {
#           if defined(TRACE)
        std::cerr << "Broadcast de " << nbItems << " objects at adress "
                  << (void*)bufsnd << " to adress " << (void*)bufrcv
                  << " with root = " << root << "\n";
#           endif
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
      
      template<typename K> void broadcast( const K* obj_snd, K& obj_rcv, int root ) const
      {
        Communication<K,is_container<K>::value>::broadcast(m_communicator, obj_snd,
                                                           obj_rcv, root);
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
        // .............................................................        
        void barrier() const
        {
            MPI_Barrier(m_communicator);
        }
        // .............................................................
        template<typename K> void
        reduce( std::size_t nbItems, const K* objs, K* res, Operation op,
                int root )
        {
            assert(objs != nullptr);
            if (root == getRank()) {
                assert(res != nullptr);
                if ( objs == res ) {
                    MPI_Reduce( MPI_IN_PLACE, res, nbItems, 
                                Type_MPI<K>::mpi_type(), op, root, m_communicator);
                } else {
                    MPI_Reduce( objs, res, nbItems, 
                                Type_MPI<K>::mpi_type(), op, root, m_communicator);                    
                }
            }
        }
        // .............................................................
        template<typename K, typename F> void
        reduce( std::size_t nbItems, const K* objs, K* res, 
                const F& fct, bool commute, int root )
        {
            assert(objs != nullptr);
            reduce_functor<K> = std::function<K(const K&, const K&)>(fct);

            MPI_Op op;
            // On prend par défaut que l'opérateur commute.
            // Il faudra sinon créer un opérateur exprès à passer
            // avec la méthode précédente.
            MPI_Op_create( reduce_user_function<K>, (commute ? 1 : 0), &op);
            
            if (root == getRank()) {
                assert(res != nullptr);
                if ( objs == res ) {
                    MPI_Reduce( MPI_IN_PLACE, res, nbItems, 
                                Type_MPI<K>::mpi_type(), op, root, m_communicator);
                } else {
                    MPI_Reduce( objs, res, nbItems, 
                                Type_MPI<K>::mpi_type(), op, root, m_communicator);                    
                }
            } else
                MPI_Reduce( objs, res, nbItems, 
                            Type_MPI<K>::mpi_type(), op, root, m_communicator);                    
            
        }
      template<typename K> void reduce( const K& loc, K* glob, const Operation& op,
                                        int root ) const
      {
        Communication<K,is_container<K>::value>::reduce(m_communicator, loc, glob, op, root);
      }

    private:
        MPI_Comm m_communicator;
    };
    // -----------------------------------------------------------------
  template<typename K>
  struct Communicator::Implementation::Communication<K,true>
  {
    static void send( const MPI_Comm& com, const K& snd_arr, int dest, int tag )
    {
      std::vector<typename K::value_type,typename K::allocator_type>* snd;
      if ( std::is_base_of<std::vector<typename K::value_type,
                                       typename K::allocator_type>,K>::value ) {
        snd = (std::vector<typename K::value_type,typename K::allocator_type>*)&snd_arr;
      } else {
#       if defined(DEBUG)            
        LogTrace << "Copy container data inside a vector" << "\n";
#       endif        
        snd = new std::vector<typename K::value_type,typename K::allocator_type>(snd_arr.size());
        std::copy(snd_arr.begin(), snd_arr.end(), snd->begin());
      }
      
      if ( Type_MPI<typename K::value_type>::must_be_packed() ) {
        
          MPI_Send(snd->data(), snd->size()*sizeof(typename K::value_type),
                   MPI_BYTE, dest, tag, com );
      } else {
        MPI_Send(snd->data(), snd->size(), Type_MPI<typename K::value_type>::mpi_type(),
                 dest, tag, com );
      }
#     if defined(DEBUG)            
      LogTrace << "Send a container with " << snd->size() << " elements to " << dest
               << " with tag " << tag << "\n";
#     endif
      if ( !std::is_base_of<std::vector<typename K::value_type,
                                       typename K::allocator_type>,K>::value ) {
#       if defined(DEBUG)            
        LogTrace << "Delete temporary vector" << "\n";
#       endif        
        delete snd;
      }
    }
    // .......................................................................................
    static Request isend( const MPI_Comm& com, const K& snd_obj, int dest, int tag )
    {
      std::vector<typename K::value_type,typename K::allocator_type>* snd;
      if ( std::is_base_of<std::vector<typename K::value_type,
                                       typename K::allocator_type>,K>::value ) {
        snd = (std::vector<typename K::value_type,typename K::allocator_type>*)&snd_obj;
      } else {
#       if defined(DEBUG)            
        LogTrace << "Copy container data inside a vector" << "\n";
#       endif        
        snd = new std::vector<typename K::value_type,typename K::allocator_type>(snd_obj.size());
        std::copy(snd_obj.begin(), snd_obj.end(), snd->begin());
      }

      MPI_Request m_req;
      if ( Type_MPI<typename K::value_type>::must_be_packed() ) {
        MPI_Isend(snd->data(), snd->size()*sizeof(typename K::value_type),
                  MPI_BYTE, dest, tag, com,  &m_req);
      } else {
        MPI_Isend(snd->data(), snd->size(), Type_MPI<typename K::value_type>::mpi_type(),
                  dest, tag, com, &m_req );
      }
#     if defined(DEBUG)            
      LogTrace << "Asynchrone send for a container with " << snd_obj.size() << " elements  to "
               << dest << " with tag " << tag << "\n";
#     endif        
      
      if ( !std::is_base_of<std::vector<typename K::value_type,
                                        typename K::allocator_type>,K>::value ) {
#       if defined(DEBUG)            
        LogTrace << "Delete temporary vector" << "\n";
#       endif        
        delete snd;
      }
      return Request(m_req);            
    }
    // .......................................................................................
    static Status recv( const MPI_Comm& com, K& rcvobj, int sender, int tag )
    {
      MPI_Status status;
      MPI_Probe(sender, tag, com, &status);
      int szMsg;
      MPI_Get_count(&status, Type_MPI<typename K::value_type>::mpi_type(), &szMsg);
      std::vector<typename K::value_type,typename K::allocator_type>* rcv;
      if ( std::is_base_of<std::vector<typename K::value_type,
                                        typename K::allocator_type>,K>::value ) {
        rcv = (std::vector<typename K::value_type,typename K::allocator_type>*)&rcvobj;
        if ( rcv->size() < szMsg ) {
#         if defined(DEBUG)            
          LogTrace << "Realloc rcv vector to match receive size message" << "\n";
#         endif
          std::vector<typename K::value_type,typename K::allocator_type>(szMsg).swap(*rcv);
        }
      } else {
#       if defined(DEBUG)
        LogTrace << "Create container data to receive" << "\n";
#       endif
        rcv = new std::vector<typename K::value_type,typename K::allocator_type>(szMsg);
      }
#     if defined(DEBUG)
      LogTrace << "Receive a container with " << rcv->size() << " elements  from " <<sender
               << " with tag " << tag << "\n";
#     endif
      if ( Type_MPI<typename K::value_type>::must_be_packed() ) {
        MPI_Recv(rcv->data(), rcv->size()*sizeof(typename K::value_type), MPI_BYTE,
                 sender, tag, com, &status);
      } else {
        MPI_Recv(rcv->data(), rcv->size(), Type_MPI<typename K::value_type>::mpi_type(),
                 sender, tag, com, &status );
      }
#     if defined(DEBUG)      
      LogTrace << "OK, receive done !" << "\n";
#     endif      
      if ( !std::is_base_of<std::vector<typename K::value_type,
                                        typename K::allocator_type>,K>::value ) {
#       if defined(DEBUG)      
        LogTrace << "Copy temporary vector inside the container passed as parameter." << "\n";
#       endif        
        rcvobj = K(rcv->begin(),rcv->end());
        delete rcv;
      }
      return Status(status);
    }
    // .......................................................................................
    static Request irecv( const MPI_Comm& com, K& rcvobj, int sender, int tag )
    {
#     if defined(DEBUG)
      LogTrace << "Asynchronous receive of a container with " << rcvobj.size()
               << " elements from " << sender << " with tag " << tag << "\n";
#     endif
      std::vector<typename K::value_type,typename K::allocator_type>* rcv;
      if ( std::is_base_of<std::vector<typename K::value_type,
                                       typename K::allocator_type>,K>::value ) {
        rcv = (std::vector<typename K::value_type,typename K::allocator_type>*)&rcvobj;
      } else {
#       if defined(DEBUG)
        LogTrace << "Create container data to receive" << "\n";
#       endif
        rcv = new std::vector<typename K::value_type,typename K::allocator_type>(rcvobj.size());
      }
      
      MPI_Request req;
      if ( Type_MPI<typename K::value_type>::must_be_packed() ) {
        MPI_Irecv(rcv->data(), rcv->size()*sizeof(typename K::value_type), MPI_BYTE,
                  sender, tag, com, &req);
      } else {
        MPI_Irecv(rcv->data(), rcv->size(), Type_MPI<typename K::value_type>::mpi_type(),
                  sender, tag, com, &req );
      }

      if ( !std::is_base_of<std::vector<typename K::value_type,
                                       typename K::allocator_type>,K>::value ) {
#       if defined(DEBUG)
        LogTrace << "Copy temporary vector inside the container passed as parameter." << "\n";
#       endif        
        std::copy(rcvobj.begin(), rcvobj.end(), rcv->begin());
        delete rcv;
      }
      
      return Request(req);
    }
    // .......................................................................................
    static void broadcast( const MPI_Comm& com, const K* obj_snd, K& obj_rcv, int root )
    {
      std::size_t szMsg = ( obj_snd != nullptr ? obj_snd->size() : obj_rcv.size() );
#     if defined(DEBUG)
      LogTrace << "Broadcast of a container with " << szMsg
               << " elements with root = " << root << "\n";
#     endif      
      std::vector<typename K::value_type,typename K::allocator_type>* rcv;
      if ( std::is_base_of<std::vector<typename K::value_type,
                                       typename K::allocator_type>,K>::value ) {
        rcv = (std::vector<typename K::value_type,typename K::allocator_type>*)&obj_rcv;
        if (szMsg > rcv->size()) {
#         if defined(DEBUG)
          LogTrace << "Realloc rcv vector to match broadcast size message" << "\n";
#         endif
          std::vector<typename K::value_type,
                      typename K::allocator_type>(obj_snd->size()).swap(*rcv);            
        }
      } else {
#       if defined(DEBUG)
        LogTrace << "Create container data to receive" << "\n";
#       endif
        rcv = new std::vector<typename K::value_type,typename K::allocator_type>(szMsg);
      }
      
      int rank;
      MPI_Comm_rank(com, &rank);
      if ( (root == rank) && (&obj_rcv != obj_snd) ) {
        assert(obj_snd != nullptr);
        std::copy(obj_snd->begin(), obj_snd->end(), obj_rcv.begin());
      }
      if ( Type_MPI<typename K::value_type>::must_be_packed() ) {
        MPI_Bcast(rcv->data(), rcv->size()*sizeof(typename K::value_type), MPI_BYTE, root, com );
      } else {
        MPI_Bcast(rcv->data(), rcv->size(), Type_MPI<typename K::value_type>::mpi_type(),
                  root, com );
      }
#     if defined(DEBUG)
      LogTrace << "End of broadcasting" << "\n";
#     endif      
      if ( !std::is_base_of<std::vector<typename K::value_type,
                                        typename K::allocator_type>,K>::value ) {
#       if defined(DEBUG)
        LogTrace << "Copy temporary vector inside the container passed as parameter." << "\n";
#       endif
        std::copy(obj_rcv.begin(), obj_rcv.end(), rcv->begin());
        delete rcv;
      }

    }
    // .......................................................................................
    static void reduce( const MPI_Comm& com, const K& loc, K* glob,
                        const Operation& op, int root ) {
      std::size_t szMsg = loc.size();
      int rank;
      MPI_Comm_rank(com, &rank);
#     if defined(DEBUG)
      LogTrace << "Reduce operation on one container with " << szMsg
               << " elements with root = " << root << "\n";
#     endif
      std::vector<typename K::value_type,typename K::allocator_type>* glb;
      std::vector<typename K::value_type,typename K::allocator_type>* lc;
      if ( std::is_base_of<std::vector<typename K::value_type,
                                       typename K::allocator_type>,K>::value ) {
        glb = (std::vector<typename K::value_type,typename K::allocator_type>*)glob;
        lc  = (const std::vector<typename K::value_type,typename K::allocator_type>*)&loc;
        if ( glb != nullptr ) {
          if (szMsg > glb->size()) {
#           if defined(DEBUG)
            LogTrace << "Realloc glb vector to match reduce size message" << "\n";
#           endif
            std::vector<typename K::value_type,
                        typename K::allocator_type>(szMsg).swap(*glb);
          }
        }
      } else {
#       if defined(DEBUG)
        LogTrace << "Create container data to compute" << "\n";
#       endif
        if ( rank == root )
          glb = new std::vector<typename K::value_type,typename K::allocator_type>(szMsg);
        else
          glb = nullptr;
        lc  = new std::vector<typename K::value_type,typename K::allocator_type>(szMsg);
        std::copy(loc.begin(), loc.end(), lc->begin());
      }
      if ( glb != nullptr )
        MPI_Reduce( lc->data(), glb->data(), szMsg, Type_MPI<typename K::value_type>::mpi_type(),
                    op, root, com );
      else
        MPI_Reduce( lc->data(), nullptr, szMsg, Type_MPI<typename K::value_type>::mpi_type(),
                    op, root, com );
#     if defined(DEBUG)
      LogTrace << "End of reduction" << "\n";
#     endif
      if ( rank == root )
        if ( !std::is_base_of<std::vector<typename K::value_type,
                                          typename K::allocator_type>,K>::value ) {
          std::copy( glb->begin(), glb->end(), glob->begin() );
          delete glb;
          delete lc;
        }
    }
    // Continue for reduce and reduce_all
  };      

}


#endif
