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
#ifndef _PARALLEL_LOGGER_HPP_
# define _PARALLEL_LOGGER_HPP_
# include <mutex>
# include <list>
# include <iostream>
# include <sstream>

namespace Parallel
{
  class Logger
  {
  public:
    class Listener
    {
    public:
      enum  {
        Listen_for_nothing     = 0,
        Listen_for_assertion   = 1,
        Listen_for_error       = 2,
        Listen_for_warning     = 4,
        Listen_for_information = 8,
        Listen_for_trace       = 16,
        
        Listen_for_all         = 0xFFFF };

      Listener( int flags = Listen_for_nothing );
      virtual ~Listener();

      virtual std::ostream& report () = 0;
      bool toReport( int mode ) const {
        return m_flags & mode;
      }
    private:
      int m_flags;
    };

    Logger();
    
    Logger(const Logger& log) = delete;
    Logger( Logger&& log ) = delete;
    ~Logger() { flush(); }

    Logger& operator = ( const Logger& ) = delete;
    Logger& operator = ( Logger&& ) = delete;

    bool subscribe  (Listener* listener);
    bool unsubscribe(Listener* listener);

    Logger& operator [] ( int mode )
    {
      m_current_mode = mode;
      return *this;
    }
    Logger& flush()
    {
      for ( auto listener : m_listeners )
        listener->report().flush();
      return *this;
    }
    // ..........................................................................
    template<typename K> inline Parallel::Logger&
    operator << ( const K& obj )
    {
      for ( auto listener : m_listeners )
        if ( listener->toReport(m_current_mode) ) {
          listener->report() << obj;
        }
      return *this;
    }

    enum {
      NOTHING     = Listener::Listen_for_nothing,
      ASSERTION   = Listener::Listen_for_assertion,
      ERROR       = Listener::Listen_for_error,
      WARNING     = Listener::Listen_for_warning,
      INFORMATION = Listener::Listen_for_information,
      TRACE       = Listener::Listen_for_trace,
      ALL         = Listener::Listen_for_all
    };
    
    int m_rank;
    typedef Logger& (*Logger_manip)(Logger &);
    Logger& operator<<(Logger_manip manip) { return manip(*this); }
  private:
    int m_current_mode;
    std::list<Listener*> m_listeners;
  };

  
# define Mode (cond) \
  ((cond) ? Parallel::Logger::ASSERTION : Parallel::Logger::NOTHING)

# define LogAssert( cond )                                             \
    Parallel::Context::logger[Mode(cond)] << Parallel::Context::logger.m_rank << " : [ [Assertion] " \
                                    << std::string(__FILE__) << " in "                         \
                 << std::string(__FUNCTION__) << " at " << std::to_string(__LINE__) << " ] : "

# define LogWarning                                           \
  Parallel::Context::logger[Parallel::Logger::WARNING] << Parallel::Context::logger.m_rank \
  << " : [ [Warning] " << std::string(__FILE__)                            \
  << " in " << std::string(__FUNCTION__) << " at " <<  std::to_string(__LINE__)  << " ] : "

# define LogError \
  Parallel::Context::logger[Parallel::Logger::ERROR] << Parallel::Context::logger.m_rank \
  << " : [ [Error] " << std::string(__FILE__)                            \
  << " in " << std::string(__FUNCTION__) << " at " <<  std::to_string(__LINE__)  << " ] : "

# define LogInformation \
  Parallel::Context::logger[Parallel::Logger::INFORMATION] << Parallel::Context::logger.m_rank \
  << " : [Information] "
  
# define LogTrace \
    Parallel::Context::logger[Parallel::Logger::TRACE] << Parallel::Context::logger.m_rank \
  << " : [ [Trace] " << std::string(__FILE__)                            \
  << " in " << std::string(__FUNCTION__) << " at " <<  std::to_string(__LINE__)  << " ] : "

}

namespace std {
  inline Parallel::Logger& endl(Parallel::Logger & out) { out << "\n"; out.flush(); return out; }
}

#endif
