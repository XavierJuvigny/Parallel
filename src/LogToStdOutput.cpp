# include <iostream>
# include "Parallel/LogToStdOutput.hpp"
using namespace Parallel;

LogToStdOutput::LogToStdOutput( int flags ) : Logger::Listener(flags)
{}
// -------------------------------------------------------------------
std::ostream&
LogToStdOutput::report()
{
  return std::cout;
}
