#ifndef KZN_VK_DEVICE_FEATURES_HPP
#define KZN_VK_DEVICE_FEATURES_HPP

#include <vulkan/vulkan.h>
#include <initializer_list>

namespace kzn::vk
{
    enum class DeviceFeature
    {
        RobustBufferAccess,
        FullDrawIndexUint32,
        ImageCubeArray,
        IndependentBlend,
        GeometryShader,
        TessellationShader,
        SampleRateShading,
        DualSrcBlend,
        LogicOp,
        MultiDrawIndirect,
        DrawIndirectFirstInstance,
        DepthClamp,
        DepthBiasClamp,
        FillModeNonSolid,
        DepthBounds,
        WideLines,
        LargePoints,
        AlphaToOne,
        MultiViewport,
        SamplerAnisotropy,
        TextureCompressionETC2,
        TextureCompressionASTC_LDR,
        TextureCompressionBC,
        OcclusionQueryPrecise,
        PipelineStatisticsQuery,
        VertexPipelineStoresAndAtomics,
        FragmentStoresAndAtomics,
        ShaderTessellationAndGeometryPointSize,
        ShaderImageGatherExtended,
        ShaderStorageImageExtendedFormats,
        ShaderStorageImageMultisample,
        ShaderStorageImageReadWithoutFormat,
        ShaderStorageImageWriteWithoutFormat,
        ShaderUniformBufferArrayDynamicIndexing,
        ShaderSampledImageArrayDynamicIndexing,
        ShaderStorageBufferArrayDynamicIndexing,
        ShaderStorageImageArrayDynamicIndexing,
        ShaderClipDistance,
        ShaderCullDistance,
        ShaderFloat64,
        ShaderInt64,
        ShaderInt16,
        ShaderResourceResidency,
        ShaderResourceMinLod,
        SparseBinding,
        SparseResidencyBuffer,
        SparseResidencyImage2D,
        SparseResidencyImage3D,
        SparseResidency2Samples,
        SparseResidency4Samples,
        SparseResidency8Samples,
        SparseResidency16Samples,
        SparseResidencyAliased,
        VariableMultisampleRate,
        InheritedQueries,
    };

    constexpr VkPhysicalDeviceFeatures as_vk_device_features(
        const std::initializer_list<DeviceFeature>& dev_features)
    {
        VkPhysicalDeviceFeatures vk_dev_features{};
        for(const auto& feature : dev_features)
        {
            switch (feature)
            {
                case DeviceFeature::RobustBufferAccess:
                    vk_dev_features.robustBufferAccess = VK_TRUE;
                    break;
                case DeviceFeature::FullDrawIndexUint32:
                    vk_dev_features.fullDrawIndexUint32 = VK_TRUE;
                    break;
                case DeviceFeature::ImageCubeArray:
                    vk_dev_features.imageCubeArray = VK_TRUE;
                    break;
                case DeviceFeature::IndependentBlend:
                    vk_dev_features.independentBlend = VK_TRUE;
                    break;
                case DeviceFeature::GeometryShader:
                    vk_dev_features.geometryShader = VK_TRUE;
                    break;
                case DeviceFeature::TessellationShader:
                    vk_dev_features.tessellationShader = VK_TRUE;
                    break;
                case DeviceFeature::SampleRateShading:
                    vk_dev_features.sampleRateShading = VK_TRUE;
                    break;
                case DeviceFeature::DualSrcBlend:
                    vk_dev_features.dualSrcBlend = VK_TRUE;
                    break;
                case DeviceFeature::LogicOp:
                    vk_dev_features.logicOp = VK_TRUE;
                    break;
                case DeviceFeature::MultiDrawIndirect:
                    vk_dev_features.multiDrawIndirect = VK_TRUE;
                    break;
                case DeviceFeature::DrawIndirectFirstInstance:
                    vk_dev_features.drawIndirectFirstInstance = VK_TRUE;
                    break;
                case DeviceFeature::DepthClamp:
                    vk_dev_features.depthClamp = VK_TRUE;
                    break;
                case DeviceFeature::DepthBiasClamp:
                    vk_dev_features.depthBiasClamp = VK_TRUE;
                    break;
                case DeviceFeature::FillModeNonSolid:
                    vk_dev_features.fillModeNonSolid = VK_TRUE;
                    break;
                case DeviceFeature::DepthBounds:
                    vk_dev_features.depthBounds = VK_TRUE;
                    break;
                case DeviceFeature::WideLines:
                    vk_dev_features.wideLines = VK_TRUE;
                    break;
                case DeviceFeature::LargePoints:
                    vk_dev_features.largePoints = VK_TRUE;
                    break;
                case DeviceFeature::AlphaToOne:
                    vk_dev_features.alphaToOne = VK_TRUE;
                    break;
                case DeviceFeature::MultiViewport:
                    vk_dev_features.multiViewport = VK_TRUE;
                    break;
                case DeviceFeature::SamplerAnisotropy:
                    vk_dev_features.samplerAnisotropy = VK_TRUE;
                    break;
                case DeviceFeature::TextureCompressionETC2:
                    vk_dev_features.textureCompressionETC2 = VK_TRUE;
                    break;
                case DeviceFeature::TextureCompressionASTC_LDR:
                    vk_dev_features.textureCompressionASTC_LDR = VK_TRUE;
                    break;
                case DeviceFeature::TextureCompressionBC:
                    vk_dev_features.textureCompressionBC = VK_TRUE;
                    break;
                case DeviceFeature::OcclusionQueryPrecise:
                    vk_dev_features.occlusionQueryPrecise = VK_TRUE;
                    break;
                case DeviceFeature::PipelineStatisticsQuery:
                    vk_dev_features.pipelineStatisticsQuery = VK_TRUE;
                    break;
                case DeviceFeature::VertexPipelineStoresAndAtomics:
                    vk_dev_features.vertexPipelineStoresAndAtomics = VK_TRUE;
                    break;
                case DeviceFeature::FragmentStoresAndAtomics:
                    vk_dev_features.fragmentStoresAndAtomics = VK_TRUE;
                    break;
                case DeviceFeature::ShaderTessellationAndGeometryPointSize:
                    vk_dev_features.shaderTessellationAndGeometryPointSize = VK_TRUE;
                    break;
                case DeviceFeature::ShaderImageGatherExtended:
                    vk_dev_features.shaderImageGatherExtended = VK_TRUE;
                    break;
                case DeviceFeature::ShaderStorageImageExtendedFormats:
                    vk_dev_features.shaderStorageImageExtendedFormats = VK_TRUE;
                    break;
                case DeviceFeature::ShaderStorageImageMultisample:
                    vk_dev_features.shaderStorageImageMultisample = VK_TRUE;
                    break;
                case DeviceFeature::ShaderStorageImageReadWithoutFormat:
                    vk_dev_features.shaderStorageImageReadWithoutFormat = VK_TRUE;
                    break;
                case DeviceFeature::ShaderStorageImageWriteWithoutFormat:
                    vk_dev_features.shaderStorageImageWriteWithoutFormat = VK_TRUE;
                    break;
                case DeviceFeature::ShaderUniformBufferArrayDynamicIndexing:
                    vk_dev_features.shaderUniformBufferArrayDynamicIndexing = VK_TRUE;
                    break;
                case DeviceFeature::ShaderSampledImageArrayDynamicIndexing:
                    vk_dev_features.shaderSampledImageArrayDynamicIndexing = VK_TRUE;
                    break;
                case DeviceFeature::ShaderStorageBufferArrayDynamicIndexing:
                    vk_dev_features.shaderStorageBufferArrayDynamicIndexing = VK_TRUE;
                    break;
                case DeviceFeature::ShaderStorageImageArrayDynamicIndexing:
                    vk_dev_features.shaderStorageImageArrayDynamicIndexing = VK_TRUE;
                    break;
                case DeviceFeature::ShaderClipDistance:
                    vk_dev_features.shaderClipDistance = VK_TRUE;
                    break;
                case DeviceFeature::ShaderCullDistance:
                    vk_dev_features.shaderCullDistance = VK_TRUE;
                    break;
                case DeviceFeature::ShaderFloat64:
                    vk_dev_features.shaderFloat64 = VK_TRUE;
                    break;
                case DeviceFeature::ShaderInt64:
                    vk_dev_features.shaderInt64 = VK_TRUE;
                    break;
                case DeviceFeature::ShaderInt16:
                    vk_dev_features.shaderInt16 = VK_TRUE;
                    break;
                case DeviceFeature::ShaderResourceResidency:
                    vk_dev_features.shaderResourceResidency = VK_TRUE;
                    break;
                case DeviceFeature::ShaderResourceMinLod:
                    vk_dev_features.shaderResourceMinLod = VK_TRUE;
                    break;
                case DeviceFeature::SparseBinding:
                    vk_dev_features.sparseBinding = VK_TRUE;
                    break;
                case DeviceFeature::SparseResidencyBuffer:
                    vk_dev_features.sparseResidencyBuffer = VK_TRUE;
                    break;
                case DeviceFeature::SparseResidencyImage2D:
                    vk_dev_features.sparseResidencyImage2D = VK_TRUE;
                    break;
                case DeviceFeature::SparseResidencyImage3D:
                    vk_dev_features.sparseResidencyImage3D = VK_TRUE;
                    break;
                case DeviceFeature::SparseResidency2Samples:
                    vk_dev_features.sparseResidency2Samples = VK_TRUE;
                    break;
                case DeviceFeature::SparseResidency4Samples:
                    vk_dev_features.sparseResidency4Samples = VK_TRUE;
                    break;
                case DeviceFeature::SparseResidency8Samples:
                    vk_dev_features.sparseResidency8Samples = VK_TRUE;
                    break;
                case DeviceFeature::SparseResidency16Samples:
                    vk_dev_features.sparseResidency16Samples = VK_TRUE;
                    break;
                case DeviceFeature::SparseResidencyAliased:
                    vk_dev_features.sparseResidencyAliased = VK_TRUE;
                    break;
                case DeviceFeature::VariableMultisampleRate:
                    vk_dev_features.variableMultisampleRate = VK_TRUE;
                    break;
                case DeviceFeature::InheritedQueries:
                    vk_dev_features.inheritedQueries = VK_TRUE;
                    break;
            }
        }
        return vk_dev_features;
    }
}

#endif // KZN_VK_DEVICE_FEATURES_HPP