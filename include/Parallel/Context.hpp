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
/**
 *    \file    Context.hpp
 *    \brief   Declaration file of parallel context class
 *
 *    \author  Dr. Xavier JUVIGNY
 *    \version 0.1
 *    \date    03/03/2017
 */
#ifndef _PARALLEL_CONTEXT_HPP_
# define _PARALLEL_CONTEXT_HPP_
# include <sstream>
# include <iostream>
# include <fstream>
# include "Parallel/Communicator.hpp"
# include "Parallel/Logger.hpp"

/*!  \namespace Parallel
 * 
 *   Namespace gathering the objects managing the parallel services
 */
namespace Parallel
{
  /*!   \class Context
   *    \brief Class which manages parallel context according to the
   *           choosen implementation
   *    \author  Dr. Xavier JUVIGNY
   * 
   *    One instance of this class must be created. This instance
   *    initialize the underlying library choosen for the 
   *    implementation ( MPI, PVM and so .). At the destruction of
   *    this unique object, the processus are synchronized and the
   *    parallel context is destroyed. 
   *
   */ 
  class Context
  {
  public:
    /*!
     *    \enum    thread_support
     *    \brief   Enumerate support levels for multithreads.
     *
     *    This enumeration describes the support level of the parallel
     *    library for multithreads. This level is defined by the
     *    choice of the library's user and by the support of the
     *    underlying library for multithreads.
     */
    enum thread_support {
      Single,     /*!< Only the main thread can make parallel calls */
      Funneled,   /*!< Only the thread that creates Context will make parallel calls. */
      Serialized, /*!< Only one thread will make Parallel library calls at one time. */
      Multiple    /*!< Multiple threads may call MPI at once with no restrictions. */
    };
    /*!
     *    Construction initializing the parallel context
     * 
     *    The constructor must be call only one time in the program
     *    ( in the beginning of the program by example ).
     *    This constructor provides two multithread supports :
     *    1. By default : The multiple thread support
     *    2. The Single multithread support
     *
     *     \param   nargc Number of arguments ( included the executable
     *                    name )
     *     \param   argv  Argument vector
     *     \param   isMultithreaded Boolean to choose multithread level support :
     *         True ( by default ) : Multiple thread level support
     *         False               : Single thread level support
     */
    Context(int& nargc, char* argv[], bool isMultithreaded = true );
    /*!
     *     Context constructor with fine control of multithread level support
     * 
     *     \param   nargc  Number of arguments
     *     \param   argv   Argument vector
     *     \param   thread_level_support \link thread_support multithread level support
     */
    Context(int& nargc, char* argv[], thread_support thread_level_support);
 
    /*!
     *    Destructor. Synchronize all processes and destroy the parallel context.
     */
    ~Context();
    /*!
     *     Return the actual multithread level support
     */
    thread_support levelOfThreadSupport() const
    {
      return m_provided;
    }
    static Logger logger;
  private:
    thread_support m_provided; /*!< Actual multithread level support */ 
  };

}

#endif
