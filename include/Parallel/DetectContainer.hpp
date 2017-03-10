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
# ifndef _PARALLEL_DETECTCONTAINER_HPP_
# define _PARALLEL_DETECTCONTAINER_HPP_

template<typename T>
struct has_const_iterator
{
private:
  typedef char                      yes;
  typedef struct { char array[2]; } no;

  template<typename C> static yes test(typename C::const_iterator*);
  template<typename C> static no  test(...);
public:
  static const bool value = sizeof(test<T>(0)) == sizeof(yes);
  typedef T type;
};

template <typename T>
struct has_begin_end
{
  template<typename C> static char (&f(typename std::enable_if<
                                       std::is_same<decltype(static_cast<typename C::const_iterator (C::*)() const>(&C::begin)),
                                       typename C::const_iterator(C::*)() const>::value, void>::type*))[1];

  template<typename C> static char (&f(...))[2];

  template<typename C> static char (&g(typename std::enable_if<
                                       std::is_same<decltype(static_cast<typename C::const_iterator (C::*)() const>(&C::end)),
                                       typename C::const_iterator(C::*)() const>::value, void>::type*))[1];
  
  template<typename C> static char (&g(...))[2];

  static bool const beg_value = sizeof(f<T>(0)) == 1;
  static bool const end_value = sizeof(g<T>(0)) == 1;
};
template<typename T> 
struct is_container : std::integral_constant<bool, has_const_iterator<T>::value && has_begin_end<T>::beg_value && has_begin_end<T>::end_value> 
{ };


#endif
