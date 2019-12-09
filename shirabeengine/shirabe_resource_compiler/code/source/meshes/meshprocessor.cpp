#include "meshes/meshprocessor.h"

#include <tuple>
#include <fx/gltf.h>
#include <core/databuffer.h>
#include <core/result.h>
#include <core/helpers.h>
#include "common/definition.h"

//
// Created by dotti on 09.12.19.
//
namespace meshes
{
    using engine::ByteBuffer;
    using engine::CResult;
    using resource_compiler::EResult;

    fx::gltf::Document const __loadFromData(ByteBuffer const &aInputBuffer)
    {
        try
        {
            nlohmann::json json;
            {
                std::string const  data = std::string(reinterpret_cast<char const*>(aInputBuffer.data()));
                std::istringstream stream(data);
                stream >> json;
            }

            return fx::gltf::detail::Create(json, { });
        }
        catch (fx::gltf::invalid_gltf_document &)
        {
            throw;
        }
        catch (std::system_error &)
        {
            throw;
        }
        catch (...)
        {
            std::throw_with_nested(fx::gltf::invalid_gltf_document("Invalid glTF document. See nested exception for details."));
        }
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    static uint32_t __calculateDataTypeSize(fx::gltf::Accessor const &aAccessor) noexcept
    {
        uint32_t elementSize = 0;
        switch (aAccessor.componentType)
        {
            case fx::gltf::Accessor::ComponentType::Byte:
            case fx::gltf::Accessor::ComponentType::UnsignedByte:
                elementSize = 1;
                break;
            case fx::gltf::Accessor::ComponentType::Short:
            case fx::gltf::Accessor::ComponentType::UnsignedShort:
                elementSize = 2;
                break;
            case fx::gltf::Accessor::ComponentType::Float:
            case fx::gltf::Accessor::ComponentType::UnsignedInt:
                elementSize = 4;
                break;
        }

        switch (aAccessor.type)
        {
            case fx::gltf::Accessor::Type::Mat2:
                return (4 * elementSize);
            case fx::gltf::Accessor::Type::Mat3:
                return (9 * elementSize);
            case fx::gltf::Accessor::Type::Mat4:
                return (16 * elementSize);
            case fx::gltf::Accessor::Type::Scalar:
                return (elementSize);
            case fx::gltf::Accessor::Type::Vec2:
                return (2 * elementSize);
            case fx::gltf::Accessor::Type::Vec3:
                return (3 * elementSize);
            case fx::gltf::Accessor::Type::Vec4:
                return (4 * elementSize);
        }

        return 0;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    static std::tuple<fx::gltf::Accessor   const &
                    , fx::gltf::BufferView const &
                    , fx::gltf::Buffer     const &
                    , uint8_t const * // data
                    , uint64_t        // stride
                    , uint64_t        // totalSize
            >
        __getData(fx::gltf::Document const &aDocument
                , uint64_t           const &aAccessorIndex)
    {
        fx::gltf::Accessor   const &accessor   = aDocument.accessors  [aAccessorIndex];
        fx::gltf::BufferView const &bufferView = aDocument.bufferViews[accessor.bufferView];
        fx::gltf::Buffer     const &buffer     = aDocument.buffers    [bufferView.buffer];

        uint32_t const dataTypeSize = __calculateDataTypeSize(accessor);

        uint8_t const *data = &(buffer.data[static_cast<uint64_t>(bufferView.byteOffset) + accessor.byteOffset]);

        return { accessor, bufferView, buffer, data, dataTypeSize, (accessor.count * dataTypeSize) };
    }

    CResult<EResult> processMesh(std::filesystem::path const &aMeshFile, SConfiguration const &aConfig)
    {
        fx::gltf::Document document = fx::gltf::LoadFromText(aMeshFile);

        std::vector<uint8_t> positions;
        std::vector<uint8_t> normals;
        std::vector<uint8_t> tangents;
        std::vector<uint8_t> uvcoordinates;
        std::vector<uint8_t> indices;

        std::vector<uint8_t> attributeBuffer;
        std::vector<uint8_t> indexBuffer;

        auto const append = [] (std::vector<uint8_t>       &aTargetBuffer
                                , fx::gltf::Accessor   const &aAccessor
                                , uint8_t              const *aSourceData
                                , uint32_t             const &aStride)
        {
            std::size_t const previousSize   = aTargetBuffer.size();
            std::size_t const additionalSize = (aAccessor.count / aStride);

            aTargetBuffer.resize(previousSize + additionalSize);
            memcpy(aTargetBuffer.data() + previousSize, aSourceData, additionalSize);
        };

        for(auto const &mesh : document.meshes)
        {
            for(auto const &primitive : mesh.primitives)
            {
                for(auto const &[attributeName, accessorIndex] : primitive.attributes)
                {
                    auto const &[accessor, bufferView, buffer, data, stride, totalSize] = __getData(document, accessorIndex);

                    if("POSITION"   == attributeName) append(positions,     accessor, data, stride);
                    if("NORMAL"     == attributeName) append(normals,       accessor, data, stride);
                    if("TANGENT"    == attributeName) append(tangents,      accessor, data, stride);
                    if("TEXCOORD_0" == attributeName) append(uvcoordinates, accessor, data, stride);
                }

                auto const &[accessor, bufferView, buffer, data, stride, totalSize] = __getData(document, primitive.indices);
                append(indices, accessor, data, stride);
            }
        }

        //
        // For now, let's assume, that indices are in monotonuous increasing order across primitives...
        //
        attributeBuffer.insert(attributeBuffer.end(), positions    .begin(), positions    .end());
        attributeBuffer.insert(attributeBuffer.end(), normals      .begin(), normals      .end());
        attributeBuffer.insert(attributeBuffer.end(), tangents     .begin(), tangents     .end());
        attributeBuffer.insert(attributeBuffer.end(), uvcoordinates.begin(), uvcoordinates.end());

        indexBuffer = indices;

        engine::writeFile( (aConfig.outputPath / "Test.attributes").string(), attributeBuffer);
        engine::writeFile( (aConfig.outputPath / "Test.indices").string(),    indexBuffer);

        return EResult::Success;
    }
}
