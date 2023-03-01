#include "vk/descriptor_set.hpp"

#include <memory>
#include "core/log.hpp"

namespace kzn::vk {
    DescriptorSetLayoutBuilder& DescriptorSetLayoutBuilder::add_uniform(
        uint32_t binding,
        VkShaderStageFlags stage_flags)
    {
        m_layout_bindings.push_back(VkDescriptorSetLayoutBinding{
            .binding = binding,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = stage_flags,
            .pImmutableSamplers = nullptr,
        });

        return *this;
    }

    DescriptorSetLayout DescriptorSetLayoutBuilder::build(DescriptorSetLayoutCache& _cache) {
        return _cache.create_layout(std::move(m_layout_bindings));
    }


    VkDescriptorPool create_pool(
        Device& device,
        const DescriptorSetAllocator::PoolSizes& pool_sizes,
        uint32_t count,
        VkDescriptorPoolCreateFlags flags)
    {
        std::vector<VkDescriptorPoolSize> sizes;
        sizes.reserve(pool_sizes.size());
        for (auto sz : pool_sizes) {
            sizes.push_back({ sz.first, uint32_t(sz.second * count) });
        }
        VkDescriptorPoolCreateInfo pool_info = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .flags = flags,
            .maxSets = count,
            .poolSizeCount = static_cast<uint32_t>(sizes.size()),
            .pPoolSizes = sizes.data(),
        };

        VkDescriptorPool descriptor_pool;
        vkCreateDescriptorPool(device.vk_device(), &pool_info, nullptr, &descriptor_pool);

        return descriptor_pool;
    }


    DescriptorSetAllocator::DescriptorSetAllocator(Device* _device)
        : device(_device)
        , descriptor_sizes{
            { VK_DESCRIPTOR_TYPE_SAMPLER, 0.5f },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4.f },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 4.f },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1.f },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1.f },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1.f },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2.f },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2.f },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1.f },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1.f },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 0.5f }
        }
    {
        used_pools.reserve(5);
        free_pools.reserve(5);
    }


    DescriptorSetAllocator::~DescriptorSetAllocator() {
        // Delete every pool held
        for(auto p : free_pools) {
            vkDestroyDescriptorPool(device->vk_device(), p, nullptr);
        }
        for(auto p : used_pools) {
            vkDestroyDescriptorPool(device->vk_device(), p, nullptr);
        }
    }


    void DescriptorSetAllocator::reset_pools() {
        for (auto& p : used_pools) {
            vkResetDescriptorPool(device->vk_device(), p, 0);
        }

        free_pools = std::move(used_pools);
        used_pools.clear();
        current_pool = VK_NULL_HANDLE;
    }


    VkDescriptorSet DescriptorSetAllocator::allocate(const VkDescriptorSetLayout& layout) {
        if(current_pool == VK_NULL_HANDLE) {
            current_pool = grab_pool();
            used_pools.push_back(current_pool);
        }

        VkDescriptorSetAllocateInfo alloc_info{
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = current_pool,
            .descriptorSetCount = 1,
            .pSetLayouts = &layout,
        };
        
        VkDescriptorSet set;
        auto result = vkAllocateDescriptorSets(device->vk_device(), &alloc_info, &set);

        if(result == VK_SUCCESS) [[likely]] {
            return set;
        }
        else [[unlikely]] {
            if(result == VK_ERROR_FRAGMENTED_POOL
                || result == VK_ERROR_OUT_OF_POOL_MEMORY) [[likely]]
            {
                // Needs allocation of a
                // new pool and retry
                current_pool = grab_pool();
                used_pools.push_back(current_pool);

                result = vkAllocateDescriptorSets(device->vk_device(), &alloc_info, &set);
                VK_CHECK_MSG(result, "DescriptorSet allocation failed on new pool");
                return set;
            }
        }

        throw ResultError(result);
    }


    VkDescriptorPool DescriptorSetAllocator::grab_pool() {
        if (free_pools.size() > 0) {
            VkDescriptorPool pool = free_pools.back();
            free_pools.pop_back();
            return pool;
        }
        else {
            return create_pool(*device, descriptor_sizes, 1000, 0);
        }
    }


    bool DescriptorSetLayoutCache::DescriptorLayoutInfo::operator==(const DescriptorLayoutInfo& other) const {
        if (other.bindings.size() != bindings.size()) {
            return false;
        }
        else {
            // Compare each of the bindings is the same. Bindings are sorted so they will match
            for(size_t i = 0; i < bindings.size(); i++) {
                if(other.bindings[i].binding != bindings[i].binding
                    || other.bindings[i].descriptorType != bindings[i].descriptorType
                    || other.bindings[i].descriptorCount != bindings[i].descriptorCount
                    || other.bindings[i].stageFlags != bindings[i].stageFlags)
                {
                    return false;
                }
            }
            return true;
        }
    }


    size_t DescriptorSetLayoutCache::DescriptorLayoutInfo::hash() const
    {
        using std::size_t;
        using std::hash;

        size_t result = std::hash<std::size_t>()(bindings.size());

        for(const auto& b : bindings) {
            // Pack the binding data into a single int64. Not fully correct but its ok
            std::size_t binding_hash = b.binding
                | b.descriptorType << 8
                | b.descriptorCount << 16
                | b.stageFlags << 24;

            // Shuffle the packed binding data and xor it with the main hash
            result ^= hash<size_t>()(binding_hash);
        }

        return result;
    }


    DescriptorSetLayoutCache::DescriptorSetLayoutCache(Device* _device)
        : device(_device) { }


    DescriptorSetLayoutCache::~DescriptorSetLayoutCache()
    {
        for (auto& [_, layout] : layout_cache) {
            vkDestroyDescriptorSetLayout(device->vk_device(), layout, nullptr);
        }
    }


    DescriptorSetLayout DescriptorSetLayoutCache::create_layout(std::vector<VkDescriptorSetLayoutBinding>&& bindings) {
        auto layout_info = DescriptorLayoutInfo {
            .bindings = std::move(bindings),
        };
        std::sort(
            layout_info.bindings.begin(),
            layout_info.bindings.end(),
            [](const auto& a, const auto& b ) {
                return a.binding < b.binding;
            }
        );

        auto it = layout_cache.find(layout_info);
        if (it != layout_cache.end())
        {
            // Return non-owning layout
            return DescriptorSetLayout(
                std::span(
                    layout_info.bindings.begin(),
                    layout_info.bindings.size()),
                    it->second
            );
        }
        else
        {
            // Create vulkan layout
            VkDescriptorSetLayout layout;
            VkDescriptorSetLayoutCreateInfo layout_create_info{
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
                .bindingCount = static_cast<uint32_t>(bindings.size()),
                .pBindings = bindings.data(),
            };
            vkCreateDescriptorSetLayout(device->vk_device(), &layout_create_info, nullptr, &layout);

            // Add layout to cache
            layout_cache[std::move(layout_info)] = layout;
            
            // Return non-owning layout
            return DescriptorSetLayout(
                std::span(
                    layout_info.bindings.begin(),
                    layout_info.bindings.size()),
                    layout
            );
        }
    }


    // DescriptorSet::DescriptorSet(
    //     Device*                                         _device,
    //     DescriptorSetAllocator&                         _allocator,
    //     DescriptorSetLayoutCache&                       _cache,
    //     const std::initializer_list<vk::BufferBinding>& _bindings)
    //     : device(_device)
    // {
    //     // Unique pointer array
    //     auto layout_bindings = std::make_unique<VkDescriptorSetLayoutBinding[]>(_bindings.size());
    //     auto writes = std::make_unique<VkWriteDescriptorSet[]>(_bindings.size());
    //     size_t i = 0;
    //     for(const auto& binding : _bindings) {
    //         layout_bindings[i] = VkDescriptorSetLayoutBinding{
    //             .binding = binding.binding,
    //             .descriptorType = binding.type,
    //             .descriptorCount = 1,
    //             .stageFlags = binding.stages,
    //             .pImmutableSamplers = nullptr,
    //         };
            
    //         ++i;
    //     }
        
    //     VkDescriptorSetLayoutCreateInfo layout_info{
    //         .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
    //         .bindingCount = static_cast<uint32_t>(_bindings.size()),
    //         .pBindings = layout_bindings.get(),
    //     };
        
    //     // Use descreiptor layour cache to create the layout if
    //     // it isn't created already
    //     set_layout = _cache.create_layout(&layout_info);
    //     // Use automaticly managed descriptor set allocator
    //     vk_descriptor_set = _allocator.allocate(set_layout);


    //     i = 0;
    //     for(const auto& binding : _bindings) {
    //         // Image sampler
    //         if(binding.type == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
    //             writes[i] = VkWriteDescriptorSet{
    //                 .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
    //                 .dstSet = vk_descriptor_set,
    //                 .dstBinding = binding.binding,
    //                 // .dstArrayElement = 0,
    //                 .descriptorCount = 1,
    //                 .descriptorType = binding.type,
    //                 .pImageInfo = &binding.image_info,
    //             };
    //         }
    //         // Uniform
    //         else { 
    //             writes[i] = VkWriteDescriptorSet{
    //                 .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
    //                 .dstSet = vk_descriptor_set,
    //                 .dstBinding = binding.binding,
    //                 // .dstArrayElement = 0,
    //                 .descriptorCount = 1,
    //                 .descriptorType = binding.type,
    //                 .pBufferInfo = &binding.buffer_info,
    //             };
    //         }
    //         ++i;
    //     }

    //     // device->vk_device() must be the same as the one allocator uses
    //     vkUpdateDescriptorSets(
    //         device->vk_device(),
    //         static_cast<uint32_t>(_bindings.size()),
    //         writes.get(),
    //         0,
    //         nullptr);
    // }


    // std::vector<DescriptorSet> DescriptorSet::multiple(
    //     size_t                                              _num_sets,
    //     Device*                                             _device,
    //     DescriptorSetAllocator&                             _allocator,
    //     DescriptorSetLayoutCache&                           _cache,
    //     const std::initializer_list<MultipleBufferBinding>& _bindings)
    // {
    //     std::vector<DescriptorSet> sets;
    //     sets.reserve(_num_sets);
    //     // For each set to be created
    //     for(size_t i = 0; i < _num_sets; ++i) {
    //         // Make a tmp vector with set's unique buffer bindings
    //         std::vector<BufferBinding> tmp;
    //         tmp.reserve(_num_sets);
    //         for(size_t j = 0; j < _num_sets; ++j)
    //             tmp.emplace_back(
    //                 _bindings[i].binding,
    //                 _bindings[i].info[j],
    //                 _bindings[i].type,
    //                 _bindings[i].stages);
    //         // Create set
    //         sets.emplace_back(
    //             _device,
    //             _allocator,
    //             _cache,
    //             tmp);
    //     }
    //     return sets;
    // }


    void DescriptorSet::bind(vk::CommandBuffer& cmd_buffer, VkPipelineLayout pipeline_layout) const {
        vkCmdBindDescriptorSets(
            cmd_buffer.vk_command_buffer(),
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipeline_layout,
            0,
            1,
            &m_vk_descriptor_set,
            0,
            nullptr);
    }


    DescriptorSet::DescriptorSet(
        Device*             device,
        VkDescriptorSet     descriptor_set,
        DescriptorSetLayout layout)
        : m_device(device)
        , m_vk_descriptor_set(descriptor_set)
        , m_set_layout(layout)
    {
        
    }
} // namespace kzn::vk