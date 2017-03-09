/*!
 *    \brief Operator for reduce and scan operations.
 */
#ifndef _PARALLEL_OPERATOR_HPP_
# define _PARALLEL_OPERATOR_HPP_

namespace Parallel
{
    template<typename K> class Operator
    {
    private:
        struct Implementation;
        Implementation* m_impl;
    };
}

#endif
