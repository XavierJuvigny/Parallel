// Log to file
#ifndef _PARALLEL_LOGTOFILE_HPP_
# define _PARALLEL_LOGTOFILE_HPP_
# include <string>
# include "Parallel/Logger.hpp"

namespace Parallel
{
  class LogToFile : public Logger::Listener
  {
  public:
    LogToFile( std::string const& filename_base, int flags);
    ~LogToFile();
  private:
    virtual std::ostream& report() override;
    std::string m_fileName;
    std::ofstream m_file;
  };
}
#endif
