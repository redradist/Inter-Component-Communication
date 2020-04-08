//
// Created by redra on 23.11.19.
//

#ifndef ICC_SOCKETTYPES_HPP
#define ICC_SOCKETTYPES_HPP

#include <vector>
#include <memory>

namespace icc {

namespace os {

using ChunkData = std::vector<uint8_t>;
using SharedChunkData = std::shared_ptr<std::vector<uint8_t>>;

}

}

#endif //ICC_SOCKETTYPES_HPP
