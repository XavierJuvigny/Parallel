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
# include <sstream>
# include <iomanip>
# include "Parallel/Context.hpp"
using namespace Parallel;

Logger Context::logger;


#if defined(USE_MPI)
Context::Context(int& nargc, char* argv[], bool isMultithreaded ) :
    Context::Context(nargc, argv, 
                     (isMultithreaded ? Context::thread_support::Multiple :
                                        Context::thread_support::Single ))
{}
// .....................................................................
Context::Context(int& nargc, char* argv[], 
                 Context::thread_support thread_level_support) :
    m_provided(thread_level_support)
{
#   if defined(TRACE)
    std::cerr << "Initialisation du contexte parallèle avec MPI\n";
#   endif
    if ( thread_level_support == Context::thread_support::Single)
        MPI_Init( &nargc, &argv );
    else {
#       if defined(TRACE)
        std::cerr << "\t ---> contexte multithreadé : ";
#       endif
        int level_support;
        switch(thread_level_support) {
            case thread_support::Funneled:
                level_support = MPI_THREAD_FUNNELED;
                break;
            case thread_support::Serialized:
                level_support = MPI_THREAD_SERIALIZED;
                break;
            default:
                level_support = MPI_THREAD_MULTIPLE;
        }
        int provided;
        MPI_Init_thread( &nargc, &argv, MPI_THREAD_MULTIPLE, &provided );
#       if defined(TRACE)
        int rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        if (rank == 0)
            std::cerr << " niveau de compatibilité : " << provided << "\n";
#       endif
        if ( provided < MPI_THREAD_SERIALIZED )
            throw std::runtime_error("Not found multithreaded mode for the current MPI library");
        switch(provided) {
            case MPI_THREAD_FUNNELED:
                m_provided = Context::thread_support::Funneled;
                break;
            case MPI_THREAD_SERIALIZED:
                m_provided = Context::thread_support::Serialized;
                break;
            default:
                m_provided = Context::thread_support::Multiple;
        }
    }
}
// .....................................................................
Context::~Context()
{
# if defined(DEBUG)
  LogTrace << "Arrêt du contexte sous MPI" << "\n";
# endif  
  MPI_Finalize();
}
#else
Context::Context(int& nargc, char* argv[], bool isMultithreaded ) :
    m_provided((isMultithreaded ? Context::thread_support::Multiple :
                                  Context::thread_support::Single))
{
}
//
Context::Context(int& nargc, char* argv[], 
                 Context::thread_support thread_level_support) :
    m_provided(thread_level_support)
{
}
//
Context::~Context()
{}
#endif
