#ifndef _PARALLEL_NULLSTREAM_HPP_
# define _PARALLEL_NULLSTREAM_HPP_
# include <iostream>

namespace Parallel
{
  class NullBuffer : public std::streambuf
  {
  public:
    int overflow(int c) { return c; }
  };
}

#endif
