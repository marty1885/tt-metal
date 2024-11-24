// SPDX-FileCopyrightText: © 2023 Tenstorrent Inc.
//
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include <memory>
#include <string>

#include "hostdevcommon/common_values.hpp"
#include "tt_metal/impl/allocator/algorithms/allocator_algorithm.hpp"
#include <boost/smart_ptr/local_shared_ptr.hpp>

namespace tt {
namespace tt_metal {
namespace allocator {
class FreeList : public Algorithm {
   public:
    enum class SearchPolicy {
        BEST = 0,
        FIRST = 1
    };

    FreeList(DeviceAddr max_size_bytes, DeviceAddr offset_bytes, DeviceAddr min_allocation_size, DeviceAddr alignment, SearchPolicy search_policy);
    void init();

    std::vector<std::pair<DeviceAddr, DeviceAddr>> available_addresses(DeviceAddr size_bytes) const;

    std::optional<DeviceAddr> allocate(DeviceAddr size_bytes, bool bottom_up=true, DeviceAddr address_limit=0);

    std::optional<DeviceAddr> allocate_at_address(DeviceAddr absolute_start_address, DeviceAddr size_bytes);

    void deallocate(DeviceAddr absolute_address);

    void clear();

    Statistics get_statistics() const;

    void dump_blocks(std::ofstream &out) const;

    void shrink_size(DeviceAddr shrink_size, bool bottom_up=true);

    void reset_size();

   private:
    struct Block {
        Block(DeviceAddr address, DeviceAddr size) : address(address), size(size) {}
        Block(DeviceAddr address, DeviceAddr size, Block* prev_block, Block* next_block, Block* prev_free, Block* next_free)
              : address(address), size(size), prev_block(prev_block), next_block(next_block), prev_free(prev_free), next_free(next_free) {}
        DeviceAddr address;
        DeviceAddr size;
        Block* prev_block = nullptr;
        Block* next_block = nullptr;
        Block* prev_free = nullptr;
        Block* next_free = nullptr;
    };

    void dump_block(const Block* block, std::ofstream &out) const;

    bool is_allocated(const Block* block) const;

    Block* search_best(DeviceAddr size_bytes, bool bottom_up);

    Block* search_first(DeviceAddr size_bytes, bool bottom_up);

    Block* search(DeviceAddr size_bytes, bool bottom_up);

    void allocate_entire_free_block(Block* free_block_to_allocate);

    void update_left_aligned_allocated_block_connections(Block* free_block, Block* allocated_block);

    void update_right_aligned_allocated_block_connections(Block* free_block, Block* allocated_block);

    Block* allocate_slice_of_free_block(Block* free_block, DeviceAddr offset, DeviceAddr size_bytes);

    Block* find_block(DeviceAddr address);

    void update_lowest_occupied_address();

    void update_lowest_occupied_address(DeviceAddr address);

    SearchPolicy search_policy_;
    Block* block_head_;
    Block* block_tail_;
    Block* free_block_head_;
    Block* free_block_tail_;
    std::vector<std::unique_ptr<Block>> block_holder_;
    Block* alloc_block(DeviceAddr address, DeviceAddr size, Block* prev_block, Block* next_block, Block* prev_free, Block* next_free)
    {
        block_holder_.push_back(std::make_unique<Block>(address, size, prev_block, next_block, prev_free, next_free));
        return block_holder_.back().get();
    }
    Block* alloc_block(DeviceAddr address, DeviceAddr size)
    {
        block_holder_.push_back(std::make_unique<Block>(address, size));
        return block_holder_.back().get();
    }
};

}  // namespace allocator
}  // namespace tt_metal
}  // namespace tt
