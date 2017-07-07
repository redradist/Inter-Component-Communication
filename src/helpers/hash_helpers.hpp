//
// Created by redra on 07.07.17.
//

#ifndef ICC_HASH_HELPER_HPP
#define ICC_HASH_HELPER_HPP

#include <functional>

namespace std {

struct pair_hash {
  template <typename T1, typename T2>
  std::size_t operator () (const std::pair<T1, T2> & _p) const {
    auto h1 = std::hash<T1>{}(_p.first);
    auto h2 = std::hash<T2>{}(_p.second);
    return h1 ^ h2;
  }
};

}

#endif //ICC_HASH_HELPER_HPP
