#ifndef __SHIRABE_GRAPHICSAPI_TYPES_PIPELINE_H__
#define __SHIRABE_GRAPHICSAPI_TYPES_PIPELINE_H__

#include <vector>
#include <stdint.h>

#include <vulkan/vulkan.h>

#include <base/declaration.h>
#include <math/geometric/rect.h>

#include <resources/core/eresourcetype.h>
#include <resources/core/iresource.h>
#include <resources/core/resourcedomaintransfer.h>
#include <resources/core/resourcetraits.h>
#include <resources/core/requestdefaultimplementation.h>

#include "graphicsapi/resources/types/definition.h"

namespace engine
{
    namespace gfxapi
    {
        using namespace math;
        using namespace resources;

        enum class EMaterialPrimitiveTopology
        {
            PointList          = 0
            , LineList
            , LineStrip
            , TriangleList
            , TriangleStrip
            , TriangleFan
            , LineListAdj
            , LineStripAdj
            , TriangleListAdj
            , TriangleStripAdj
            , PatchList
            , MaxEnum          = 0x7FFFFFFF
        };

        enum class EMaterialPolygonMode
        {
            Fill              = 0
            , Line
            , Point
            , FillRectangleNv = 1000153000
            , MaxEnum         = 0x7FFFFFFF
        };

        enum class EMaterialCullMode
        {
            None      = 0
            , Front
            , Back
            , Both
            , MaxEnum = 0x7FFFFFFF
        };

        enum class EMaterialFrontFaceConfig
        {
            CounterClockwise = 0
            , Clockwise
            , MaxEnum        = 0x7FFFFFFF
        };

        enum class EMaterialSampleCount
        {
            Sample1    = 0x00000001
            , Sample2  = 0x00000002
            , Sample4  = 0x00000004
            , Sample8  = 0x00000008
            , Sample16 = 0x00000010
            , Sample32 = 0x00000020
            , Sample64 = 0x00000040
            , MaxEnum  = 0x7FFFFFFF
        };

        enum class EMaterialComponentFlags
        {
            ComponentR   = 0x00000001
            , ComponentG = 0x00000002
            , ComponentB = 0x00000004
            , ComponentA = 0x00000008
            , MaxEnum    = 0x7FFFFFFF
        };

        enum class EMaterialBlendOp
        {
            Add                = 0
            , Subtract
            , ReverseSubtract
            , Min
            , Max
            , MaxEnum          = 0x7FFFFFFF
        };

        enum class EMaterialBlendFactor
        {
            Zero                        = 0
            , One
            , SourceColor
            , OneMinusSourceColor
            , DestinationColor
            , OneMinusDestinationColor
            , SourceAlpha
            , OneMinusSourceAlpha
            , DestinationAlpha
            , OneMinusDestinationAlpha
            , ConstantColor
            , OneMinusConstantColor
            , SourceAlphaSaturate
            , Source1Color
            , OneMinusSource1Color
            , Source1Alpha
            , OneMinusSource1Alpha
            , MaxEnum                   = 0x7FFFFFFF
        };

        enum class EMaterialLogicOp
        {
            Clear          = 0
            , And
            , AndReverse
            , Copy
            , AndInverted
            , NoOp
            , XOr
            , Or
            , NOr
            , Equivalent
            , Invert
            , OrReverse
            , CopyInverted
            , OrInverted
            , NAnd
            , Set
            , MaxEnum      = 0x7FFFFFFF
        };

        enum class EMaterialCompareOp
        {
            Never            = 0
            , Less
            , Equal
            , LessOrEqual
            , Greater
            , GreaterOrEqual
            , Always
            , MaxEnum        = 0x7FFFFFFF
        };

        enum class EMaterialStencilOp
        {
            Keep             = 0
            , Zero
            , Replace
            , IncrementClamp
            , DecrementClamp
            , Invert
            , IncrementWrap
            , DecrementWrap
            , MaxEnum        = 0x7FFFFFFF
        };

        enum class EMaterialBindingType
        {
            Sampler                   = 0
            , CombinedImageSampler
            , SampledImage
            , StorageImage
            , UniformTexelBuffer
            , StorageTexelBuffer
            , UniformBuffer
            , StorageBuffer
            , UniformBufferDynamic
            , StorageBufferDynamic
            , InputAttachment
            , InlineUniformBlockExt   = 1000138000
            , AccelerationStructureNv = 1000165000
            , MaxEnum                 = 0x7FFFFFFF
        };

        enum class EMaterialShaderStageFlags
        {
            Vertex                  = 0x00000001
            , TesselationControl    = 0x00000002
            , TesselationEvaluation = 0x00000004
            , Geometry              = 0x00000008
            , Fragment              = 0x00000010
            , Compute               = 0x00000020
            , AllGraphics           = 0x0000001F
            , All                   = 0x7FFFFFFF
            , RayGenNv              = 0x00000100
            , AnyHitNv              = 0x00000200
            , ClosestHitNv          = 0x00000400
            , MissNv                = 0x00000800
            , IntersectionNv        = 0x00001000
            , CallableNv            = 0x00002000
            , TaskNv                = 0x00000040
            , MeshNv                = 0x00000080
            , MaxEnm                = 0x7FFFFFFF
        };

        enum class EMaterialVertexInputRate
        {
            Vertex     = 0
            , Instance
            , MaxEnum  = 0x7FFFFFFF
        };

        struct SViewPort
        {
            float
                offsetX,  offsetY,
                width,    height,
                minDepth, maxDepth;
        };

        // Buffer, one per element...
        struct SMaterialVertexInputBinding
        {
            uint32_t                 binding;
            uint32_t                 stride;
            EMaterialVertexInputRate inputRate;
        };

        // Element in Buffer
        struct SMaterialVertexAttributeDescription
        {
            uint32_t binding;
            uint32_t location;
            EFormat  format;
            uint32_t byteOffset;
        };

        struct SMaterialVertexInputState
        {
            std::vector<SMaterialVertexInputBinding>         bufferBindings;
            std::vector<SMaterialVertexAttributeDescription> attributeBindings;
        };

        struct SMaterialInputAssemblyState
        {
        public_members:
            EMaterialPrimitiveTopology topology;
        };


        struct SMaterialViewPortState
        {
            std::vector<SViewPort> viewports;
            std::vector<CRect>     scissors;
        };

        struct SMaterialRasterizerState
        {
            bool                     depthClampEnable;
            bool                     depthBiasEnable;
            float                    depthBiasConstFactor;
            float                    depthBiasClamp;
            float                    depthBiasSlopeFactor;
            bool                     rasterizerDiscardEnable;
            float                    lineWidth;
            EMaterialPolygonMode     polygonMode;
            EMaterialCullMode        cullMode;
            EMaterialFrontFaceConfig frontFaceConfig;
        };

        struct SMaterialMultisamplingState
        {
            EMaterialSampleCount  samples;
            float                 minSampleShading;
            bool                  alphaToCoverageEnable;
            bool                  alphaToOneEnable;
            std::vector<uint32_t> sampleMask;
        };

        struct SMaterialStencilOpState
        {
            EMaterialStencilOp failOp;
            EMaterialStencilOp passOp;
            EMaterialStencilOp depthFailOp;
            EMaterialCompareOp compareOp;
            uint32_t           compareMask;
            uint32_t           writeMask;
            uint32_t           stencilReferenceValue;
        };

        struct SMaterialDepthStencilState
        {

            bool                    depthTestEnable;
            bool                    depthWriteEnable;
            bool                    depthBoundsTestEnable;
            bool                    stencilTestEnable;
            EMaterialCompareOp      depthCompareOp;
            SMaterialStencilOpState front;
            SMaterialStencilOpState back;
            float                   minDepthBounds;
            float                   maxDepthBounds;
        };

        struct SMaterialAttachmentBlendState
        {
            uint8_t              colorWriteMask;
            bool                 blendEnable;
            EMaterialBlendFactor blendFactorSourceColor;
            EMaterialBlendFactor blendFactorDestinationColor;
            EMaterialBlendOp     blendOpColor;
            EMaterialBlendFactor blendFactorSourceAlpha;
            EMaterialBlendFactor blendFactorDestinationAlpha;
            EMaterialBlendOp     blendOpAlpha;
        };

        struct SMaterialBlendState
        {
            bool                                       logicOpEnable;
            EMaterialLogicOp                           logicOp;
            uint32_t                                   attachmentCount;
            std::vector<SMaterialAttachmentBlendState> attachmentBlendStates;
            float                                      blendConstants[4];
        };

        struct SMaterialBinding
        {
            uint32_t                  index;
            EMaterialBindingType      descriptorType;
            uint32_t                  descriptorCount;
            EMaterialShaderStageFlags stageFlags;
        };

        struct SMaterialSet
        {
            uint32_t                      index;
            std::vector<SMaterialBinding> bindings;
        };

        /**
         * The frame graph material struct encapsulates information on
         * the material used for rendering.
         */
        struct SMaterial
        {
        public_members:
            SMaterialVertexInputState   vertexInputState;
            SMaterialInputAssemblyState inputAssemblyState;
            SMaterialViewPortState      viewPortState;
            SMaterialRasterizerState    rasterizerState;
            SMaterialDepthStencilState  depthStencilState;
            SMaterialBlendState         blendState;
            std::vector<SMaterialSet>   dataSets;
            // pipelineLayoutInfo.pushConstantRangeCount = 0;       // Optional
            // pipelineLayoutInfo.pPushConstantRanges    = nullptr; // Optional
            Vector<PublicResourceId_t>  textures;
            Vector<PublicResourceId_t>  buffers;
        };

        /**
         * The CFrameBufferDeclaration class describes a CFrameBuffer's integration into the resource/backend.
         */
        class SHIRABE_TEST_EXPORT CPipelineDeclaration
        {
        public_static_constants:
            static const constexpr EResourceType    sResourceType    = EResourceType   ::GAPI_COMPONENT;
            static const constexpr EResourceSubType sResourceSubtype = EResourceSubType::PIPELINE;

        public_structs:
            /**
             * The SDescriptor struct provides all necessary information about a texture view.
             */
            struct SHIRABE_TEST_EXPORT SDescriptor
                    : public SDescriptorImplBase<EResourceType::GAPI_COMPONENT, EResourceSubType::PIPELINE>
            {
            public_constructors:
                /**
                 * Default-Construct an empty SDescriptor.
                 */
                SDescriptor();

            public_methods:
                /**
                 * Describe this descriptor as string.
                 *
                 * @return See brief.
                 */
                std::string toString() const;

            public_members:
                std::string                                            name;
                VkViewport                                             viewPort;
                VkRect2D                                               scissor;
                VkPipelineInputAssemblyStateCreateInfo                 inputAssemblyState;
                std::vector<VkVertexInputBindingDescription>           vertexInputBindings;
                std::vector<VkVertexInputAttributeDescription>         vertexInputAttributes;
                VkPipelineRasterizationStateCreateInfo                 rasterizerState;
                VkPipelineMultisampleStateCreateInfo                   multiSampler;
                VkPipelineDepthStencilStateCreateInfo                  depthStencilState;
                std::vector<VkPipelineColorBlendAttachmentState>       colorBlendAttachmentStates;
                VkPipelineColorBlendStateCreateInfo                    colorBlendState;
                VkPipelineLayoutCreateInfo                             pipelineLayout;
                std::vector<VkDescriptorSetLayoutCreateInfo>           descriptorSetLayoutCreateInfos;
                std::vector<std::vector<VkDescriptorSetLayoutBinding>> descriptorSetLayoutBindings;
            };

            /**
             * The CreationRequest struct describes how a texture view should be created.
             */
            struct SHIRABE_TEST_EXPORT CCreationRequest
                    : public CBaseDeclaration::CCreationRequestBase<SDescriptor>
            {
            public_constructors:
                /**
                 * Construct a creation request from a descriptor, render pass and texture view handles.
                 *
                 * @param aDescriptor         The texture view descriptor to be used for creation.
                 * @param aRenderPassHandle   The render pass describing the usage of the frame buffer.
                 * @param aTextureViewHandles The texture handles of the texture views to be bound for the framebuffer.
                 */
                CCreationRequest(
                        SDescriptor            const &aDescriptor,
                        PublicResourceIdList_t const &aTextureViewHandles,
                        PublicResourceIdList_t const &aBufferViewHandles);

            public_methods:
                /**
                 * Return the list of associated texture view handles to be bound.
                 *
                 * @return See brief.
                 */
                PublicResourceIdList_t const &textureViewHandles() const;

                /**
                 * Return the list of associated texture view handles to be bound.
                 *
                 * @return See brief.
                 */
                PublicResourceIdList_t const &bufferViewHandles() const;

                /**
                 * Describe this creation request as string.
                 *
                 * @return See brief.
                 */
                std::string toString() const;

            private_members:
                PublicResourceIdList_t mTextureViewHandles;
                PublicResourceIdList_t mBufferViewHandles;
            };

            /**
             * TBDone
             */
            using CUpdateRequest = CBaseDeclaration::CUpdateRequestBase;

            /**
             * TBDone
             */
            using CDestructionRequest = CBaseDeclaration::CDestructionRequestBase;

            /**
             * TBDone
             */
            using CQuery = CBaseDeclaration::CQueryBase;
        };


        /**
         * FrameBuffer implementation for all texture views in the engine
         */
        class SHIRABE_TEST_EXPORT CPipeline
                : public CPipelineDeclaration
                , public CResourceDescriptorAdapter<CPipelineDeclaration::SDescriptor>
        {
        public_typedefs:
            using MyType_t = CPipeline;

        public_constructors:
            /**
             * Construct a texture view from a texture view descriptor.
             * @param aDescriptor
             */
            CPipeline(CPipeline::SDescriptor const &aDescriptor);
        };

        /**
         *
         */
        SHIRABE_DEFINE_PUBLIC_TRAITS_TYPES(CPipeline);

    }
}


#endif
