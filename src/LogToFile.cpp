# include <iostream>
# include <fstream>
# include <sstream>
# include <iomanip>
# include "Parallel/LogToFile.hpp"
# include "Parallel/Communicator.hpp"
# include "Parallel/NullStream.hpp"
using namespace Parallel;

namespace {
  std::ostream null_stream(new NullBuffer);
}

LogToFile::LogToFile( std::string const& filename_base, int flags ) :
  Logger::Listener(flags), m_fileName()
{
  Communicator com;
  std::stringstream file_name;
  file_name << filename_base << std::setfill('0') << std::setw(5) << com.rank << ".txt";
  m_fileName = std::string(file_name.str());
  m_file.open(m_fileName);
  if (!m_file) {
    std::cerr << "File creation failed. This listener will be unavailable.\n";
    // Le fichier ne peut pas être ouvert. Utiliser une chaîne de caractère nulle
    // pour Report afin qu'il sache qu'il ne peut pas ouvrir le fichier pour tenter
    // de rajouter un message.
    m_fileName = "";
  }
}

LogToFile::~LogToFile()
{
  m_file.close();
}

std::ostream&
LogToFile::report()
{
  if (m_file) return m_file;
  return null_stream;
}

