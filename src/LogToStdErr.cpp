# include <iostream>
# include "Parallel/LogToStdErr.hpp"
using namespace Parallel;

LogToStdErr::LogToStdErr( int flags ) : Logger::Listener(flags)
{}
// -------------------------------------------------------------------
std::ostream&
LogToStdErr::report()
{
  return std::cerr;
}
