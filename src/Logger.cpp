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
