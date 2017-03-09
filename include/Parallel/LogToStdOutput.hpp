// Log to std output
#ifndef _PARALELL_LOGTOSTDOUT_HPP_
# define _PARALLEL_LOGTOSTDOUT_HPP_
# include "Parallel/Logger.hpp"
namespace Parallel
{
  class LogToStdOutput : public Logger::Listener
  {
  public:
    LogToStdOutput(int flags);
  private:
    virtual std::ostream& report() override;
  };
}

#endif
