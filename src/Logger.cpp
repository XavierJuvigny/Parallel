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
# include <cassert>
# include <algorithm>
# include <iostream>
# include "Parallel/Logger.hpp"
# include "Parallel/Communicator.hpp"
using namespace Parallel;

Logger::Logger() : m_listeners(), m_rank(-1), m_current_mode(Logger::INFORMATION)
{}
// ------------------------------------------------------------------------
bool
Logger::subscribe(Logger::Listener* listener)
{
  if ( m_rank == -1 ) {
    m_rank = Communicator().rank;
  }
  if (listener == nullptr) return false;
  auto itL = std::find(m_listeners.begin(),m_listeners.end(), listener);
  if (itL != m_listeners.end()) return false;
  m_listeners.push_back(listener);
  return true;
}
// ------------------------------------------------------------------------
bool
Logger::unsubscribe(Logger::Listener* listener)
{
  if (listener == NULL) return false;
  auto itL = std::find(m_listeners.begin(),m_listeners.end(), listener);
  if (itL == m_listeners.end()) return false;
  m_listeners.remove(listener);
  return true;
}
// ========================================================================
Logger::Listener::Listener( int flags ) : m_flags(flags)
{}
// ------------------------------------------------------------------------
Logger::Listener::~Listener()
{}
