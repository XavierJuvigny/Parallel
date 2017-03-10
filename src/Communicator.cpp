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
    Communicator::Communicator( const Ext_Communicator& excom ) :
        m_impl(new Communicator::Implementation(excom))
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
    void Communicator::barrier() const
    {
        m_impl->barrier();
    }
}
