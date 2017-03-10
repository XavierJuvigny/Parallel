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

