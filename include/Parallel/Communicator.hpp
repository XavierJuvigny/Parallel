/**
 *    \file    Communicator.hpp
 *    \brief   Declaration file for the communicator class which manages the
 *             data exchanges messages in parallel.
 *
 *    \author  Dr. Xavier JUVIGNY
 *    \version 0.1
 *    \date    03/03/2017
 */#ifndef _PARALLEL_COMMUNICATOR_HPP_
# define _PARALLEL_COMMUNICATOR_HPP_
# include <vector>
# include <cstdlib>
# include "Parallel/Status.hpp"
namespace Parallel
{
    /*!   \class Communicator
     *    \brief This class manages the data message exchanges ( point to point or collective )
     *           inside a communication group.
     *    \author  Dr. Xavier JUVIGNY
     * 
     *    The communicator class create instances which group some processes
     *    together. Two ways to create an instance of communicator class :
     *    1. With the default constructor : a global communicator instance is
     *       created which contains all processes executed in the parallel session
     *    2. With a color and a key : to create a partition of the processes in
     *       several communicators.
     * 
     *    Probably than future versions of the library will provide other
     *    services to create new groups.
     *
     */ 
    class Communicator
    {
    public:
        /*!
         *   \brief Default constructor : instance a global communicator
         * 
         *   The default constructor build an instance which contains all
         *   processes executed for the parallel session.
         */
        Communicator();
        /*!
         *   \brief Split a communicator into a groupe of sub-communicators
         * 
         *   This constructor instances a new communicator by splitting
         *   the communicator \ref com into a group of sub--communicators
         *   based on the input values \ref color and \ref key. It's important
         *   to note here that the original communicator doesn't go away,
         *   but a new communicator is created on each process. 
         * 
         *   \param com   It's the communicator that will be used as the basis
         *                for the new communicators
         *   \param color Determines to which new communicator each processes
         *                will belong. All processes which pass in the same
         *                value for \ref color are assigned to the same
         *                communicator. If the color is the constant undefined,
         *                that process won't be included in any of the new
         *                communicators.
         *   \param key   Determines the ordering ( rank ) within each
         *                new communicator. The process which passes in the
         *                smallest value for \ref key will be rank 0, the
         *                next smallest will be rank 1, and so on. If there is
         *                a tie, the process that had the lower rank in the original
         *                communicator will be first.
         *   
         */
        Communicator( const Communicator& com, int color, int key );
        /*!
         *  \brief Convert a communicator coming from external library used
         *         for Parallel library in Parallel communicator.
         * 
         *  \param com  The external communicator coming from library used
         *              for implementation
         */
        Communicator( const Ext_Communicator& com );
        /*!
         *   \brief Duplicate a communicator in a new instance.
         *
         *   This constructor is used to create a new communicator that has a new
         *   communication context but contains the same group of processes 
         *   as the input communicator. 
         * 
         *   \param com Communicator to be duplicated
         */
        Communicator( const Communicator& com );
        
        Communicator( Communicator&& com ) = delete;
        /*!
         *    Destructor. Destroy the communicator in the parallel context.
         */
        ~Communicator();

        Communicator& operator = ( const Communicator& com ) = delete;
        Communicator& operator = ( Communicator&& com ) = delete;
        
        int rank; /*!< Rank of the current process inside the communicator instance */
        int size; /*!< Size of the communicator instance ( a.k.a number of processes
                       included in the communicator ) */
        
        /*!
         *    \brief Perform a blocking send to send an object to another process
         * 
         *    This method send an object \ref obj to a process \ref dest with
         *    an indentifier \ref tag ( default value 0 ). This method performs
         *    a blocking send for large objects and a non blocking send for
         *    small objects.
         * 
         *    \param obj  The object to send
         *    \param dest The rank of the destination
         *    \param tag  The message tag ( value default is zero )
         */
        template<typename K> void send( const K& obj, int dest, int tag = 0) const;
        /*!
         *    \brief Perform a blocking send to send a vector of objects to another process
         * 
         *    This method send a vector \ref arr to a process \ref dest with
         *    an indentifier \ref tag ( default value 0 ). This method performs
         *    a blocking send for large array and a non blocking send for
         *    small array.
         * 
         *    \param arr  The array to send
         *    \param dest The rank of the destination
         *    \param tag  The message tag ( value default is zero )
         */
        template<typename K> void send( const std::vector<K>& arr, int dest, int tag = 0) const;
        /*!
         *    \brief Perform a blocking send to send a buffer of objects to another process
         * 
         *    This method send a buffer \ref buff to a process \ref dest with
         *    an indentifier \ref tag ( default value 0 ). This method performs
         *    a blocking send for large array and a non blocking send for
         *    small array.
         * 
         *    \param nbObjs Number of objects contained in the buffer
         *    \param buff   The buffer to send
         *    \param dest   The rank of the destination
         *    \param tag    The message tag ( value default is zero )
         */
        template<typename K> void send( std::size_t nbObjs, const K* buff, int dest, int tag = 0) const;
        /*!
         *    \brief Perform a blocking receive to receive an object sended by another process
         * 
         *    This method receive an object \ref obj from a process \ref sender
         *    with an indentifier \ref tag ( default value any_tag ). This method
         *    performs a blocking receive. 
         * 
         *    \param obj    The target object where one receive data of the sended object
         *    \param sender The rank of the sender
         *    \param tag    The excepted message tag ( default value any_tag )
         *    \return Status The status of the received message.
         */
        template<typename K> Status recv( K& obj, int sender, int tag = any_tag  ) const;
        /*!
         *    \brief Perform a blocking receive for a vector of objects.
         * 
         *    This method performs a receive operation to receive a vector of objects
         *    from \ref sender with ref \ref tag. This method performs a blocking receive.
         * 
         *    \param arr     The target vector where store the receiving data.
         *    \param sender  The rank of the sender
         *    \param tag     The excepted message tag ( default value any_tag ) 
         *    \return Status The status of the received message.
         */
        template<typename K> Status recv( std::vector<K>& arr, int sender, int tag = any_tag) const;
        /*!
         *    \brief Perform a blocking receive for a buffer of objects.
         * 
         *    This method performs a receive operation to receive a buffer of objects
         *    from \ref sender with ref \ref tag. This method performs a blocking receive.
         * 
         *    \param nbObjs  The number of objects to receive into the buffer.
         *    \param buff    The target buffer where store the receiving data.
         *    \param sender  The rank of the sender
         *    \param tag     The excepted message tag ( default value any_tag ) 
         *    \return Status The status of the received message.
         */        
        template<typename K> Status recv( std::size_t nbObjs, K* buff, int sender, 
                                        int tag = any_tag) const;
        /*!
         *    \brief Perform a broadcast from a process to other processes.
         * 
         *    This method performs a broadcast from the root process to
         *    other processes.
         * 
         *    \param o_snd The object to broadcast.
         *    \param o_rcv The object where receive the broadcasted object.
         *    \param root  The rank of the root process
         */
        template<typename K> void bcast( const K& o_snd, K& o_rcv, int root = 0 ) const;
        /*!
         *    \brief Perform a broadcast from a process to other processes.
         * 
         *    This method performs a broadcast from the root process to
         *    other processes. Don't call this method with the root process !
         * 
         *    \param o_rcv The object where receive the broadcasted object.
         *    \param root  The rank of the root process
         */
        template<typename K> void bcast( K& o_rcv, int root = 0 ) const;
        /*!
         *    \brief Perform a broadcast from a process to other processes.
         * 
         *    This method performs a broadcast from the root process to
         *    other processes.
         *
         *    \param a_snd The array of objects to broadcast.
         *    \param a_rcv The array of objects where receive broadcasted objects
         *    \param root  The rank of the root process
         */
        template<typename K> void bcast( const std::vector<K>& a_snd, std::vector<K>& a_rcv, int root = 0 ) const;
        /*!
         *    \brief Perform a broadcast from a process to other processes.
         * 
         *    This method performs a broadcast from the root process to
         *    other processes. Don't call this method with the root process !
         *
         *    \param a_rcv The array of objects where receive broadcasted objects.
         *                 Beware, this array must have the right size before
         *                 the call of this method to receive the data.
         *    \param root  The rank of the root process
         */        
        template<typename K> void bcast( std::vector<K>& a_rcv, int root = 0 ) const;
        /*!
         *    \brief Perform a broadcast from a process to other processes.
         * 
         *    This method performs a broadcast from the root process to
         *    other processes.
         *
         *    \param Number of items to broadcast.
         *    \param b_snd The buffer of objects to broadcast.
         *    \param b_rcv The buffer of objects where receive broadcasted objects ( must be allocated
         *                 before the call of this method )
         *    \param root  The rank of the root process
         */
        template<typename K> void bcast( std::size_t nbObjs, const K* b_snd, K* b_rcv, int root = 0 ) const;
        /*!
         *    \brief Perform a broadcast from a process to other processes.
         * 
         *    This method performs a broadcast from the root process to
         *    other processes. Don't call this method with the root process !
         *
         *    \param Number of items to broadcast.
         *    \param b_rcv The buffer of objects where receive broadcasted objects ( must be allocated
         *                 before the call of this method )
         *    \param root  The rank of the root process
         */
        template<typename K> void bcast( std::size_t nbObjs, K* b_rcv, int root = 0 ) const;
    private:
        struct Implementation;
        Implementation* m_impl;
    };
}
#endif
