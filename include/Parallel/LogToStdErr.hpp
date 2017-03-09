 // Log to std error
#ifndef _PARALLEL_LOGTOSTDERR_HPP_
# define _PARALLEL_LOGTOSTDERR_HPP_
# include "Parallel/Logger.hpp"

namespace Parallel
{
  class LogToStdErr : public Logger::Listener
  {
  public:
    LogToStdErr(int flags);
  private:
    virtual std::ostream& report() override;
  };
}
#endif
