#include "meshes/meshprocessor.h"

#include <tuple>
#include <fx/gltf.h>
#include <core/databuffer.h>
#include <core/result.h>
#include <core/helpers.h>
#include <core/enginetypehelper.h>
#include <util/documents/json.h>
#include <util/crc32.h>
#include <mesh/declaration.h>

#include "common/functions.h"

//
// Created by dotti on 09.12.19.
//
namespace meshes
{
    using namespace engine;
    using engine::ByteBuffer;
    using engine::CResult;
    using resource_compiler::EResult;


    /**
     * Accept a SMaterial instance and serialize it to a JSON string.
     *
     * @param aMaterial
     * @param aOutSerializedData
     * @return
     */
    static CResult<EResult> serializeMeshMeta(mesh::SMeshMeta const &aMeta, std::string &aOutSerializedData)
    {
        using namespace resource_compiler::serialization;
        using namespace engine::documents;

        Unique<IJSONSerializer<mesh::SMeshMeta>> serializer  = makeUnique<CJSONSerializer<mesh::SMeshMeta>>();
        bool const                               initialized = serializer->initialize();
        if( not initialized )
        {
            return EResult::SerializationFailed;
        }
        CResult<Shared<serialization::ISerializer<mesh::SMeshMeta>::IResult>> const serialization = serializer->serialize(aMeta);
        if( not serialization.successful())
        {
            return EResult::SerializationFailed;
        }

        CResult<std::string> data = serialization.data()
                                                 ->asString();
        aOutSerializedData = data.data();

        bool const deinitialized = serializer->deinitialize();
        if( not deinitialized )
        {
            return EResult::SerializationFailed;
        }

        serializer = nullptr;

        CLog::Debug(logTag(), aOutSerializedData);

        return EResult::Success;
    }

    /**
     * Accept a SMaterial instance and serialize it to a JSON string.
     *
     * @param aMaterial
     * @param aOutSerializedData
     * @return
     */
    static CResult<EResult> serializeMeshDataFile(mesh::SMeshDataFile const &aDataFile, std::string &aOutSerializedData)
    {
        using namespace resource_compiler::serialization;
        using namespace engine::documents;

        Unique<IJSONSerializer<mesh::SMeshDataFile>> serializer = makeUnique<CJSONSerializer<mesh::SMeshDataFile>>();
        bool const initialized = serializer->initialize();
        if(not initialized)
        {
            return EResult::SerializationFailed;
        }
        CResult<Shared<serialization::ISerializer<mesh::SMeshDataFile>::IResult>> const serialization = serializer->serialize(aDataFile);
        if(not serialization.successful())
        {
            return EResult::SerializationFailed;
        }

        CResult<std::string> data = serialization.data()->asString();
        aOutSerializedData = data.data();

        bool const deinitialized = serializer->deinitialize();
        if(not deinitialized)
        {
            return EResult::SerializationFailed;
        }

        serializer = nullptr;

        CLog::Debug(logTag(), aOutSerializedData);

        return EResult::Success;
    }

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
        std::filesystem::path const &pathAbs    = std::filesystem::current_path() / aMeshFile;
        std::filesystem::path const &parentPath = std::filesystem::relative(aMeshFile, aConfig.inputPath).parent_path();
        std::filesystem::path const &meshID     = aMeshFile.stem();

        std::filesystem::path const outputPath                      = ( parentPath)                                                                  .lexically_normal();
        std::filesystem::path const outputMetaFilePath              = ( parentPath / (std::filesystem::path(meshID.string() + ".mesh.meta"))) .lexically_normal();
        std::filesystem::path const outputDataFilePath              = ( parentPath / (std::filesystem::path(meshID.string() + ".datafile")))  .lexically_normal();
        std::filesystem::path const outputAttributeBufferPath       = ( parentPath / (std::filesystem::path(meshID.string() + ".attributes"))).lexically_normal();
        std::filesystem::path const outputIndexBufferPath           = ( parentPath / (std::filesystem::path(meshID.string() + ".indices")))   .lexically_normal();
        std::filesystem::path const outputPathAbsolute              = (std::filesystem::current_path() / aConfig.outputPath / outputPath               ).lexically_normal();
        std::filesystem::path const outputMetaFilePathAbs           = (std::filesystem::current_path() / aConfig.outputPath / outputMetaFilePath       ).lexically_normal();
        std::filesystem::path const outputDataFilePathAbs           = (std::filesystem::current_path() / aConfig.outputPath / outputDataFilePath       ).lexically_normal();
        std::filesystem::path const outputAttributeBufferPathAbs    = (std::filesystem::current_path() / aConfig.outputPath / outputAttributeBufferPath).lexically_normal();
        std::filesystem::path const outputIndexBufferPathAbs        = (std::filesystem::current_path() / aConfig.outputPath / outputIndexBufferPath    ).lexically_normal();

        resource_compiler::checkPathExists(outputPathAbsolute);

        fx::gltf::Document document = fx::gltf::LoadFromText(aMeshFile);

        std::vector<uint8_t> positions;
        std::vector<uint8_t> normals;
        std::vector<uint8_t> tangents;
        std::vector<uint8_t> uvcoordinates;
        std::vector<uint8_t> indices;

        std::vector<uint8_t> attributeBuffer;
        std::vector<uint8_t> indexBuffer;

        struct BufferInfo
        {
            uint32_t count;
            uint64_t stride;
            uint64_t byteSize;
        } positionBufferInfo {}
        , normalBufferInfo   {}
        , tangentBufferInfo  {}
        , texcoordBufferInfo {}
        , indexBufferInfo    {};

        auto const append = [] (std::vector<uint8_t>       &aTargetBuffer
                              , fx::gltf::Accessor   const &aAccessor
                              , uint8_t              const *aSourceData
                              , uint32_t             const &aStride)
        {
            std::size_t const previousSize   = aTargetBuffer.size();
            std::size_t const additionalSize = (aAccessor.count * aStride);

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

                    if("POSITION"   == attributeName) { append(positions,     accessor, data, stride); positionBufferInfo = { .count = accessor.count, .stride = stride, .byteSize = totalSize}; }
                    if("NORMAL"     == attributeName) { append(normals,       accessor, data, stride); normalBufferInfo   = { .count = accessor.count, .stride = stride, .byteSize = totalSize}; }
                    if("TANGENT"    == attributeName) { append(tangents,      accessor, data, stride); tangentBufferInfo  = { .count = accessor.count, .stride = stride, .byteSize = totalSize}; }
                    if("TEXCOORD_0" == attributeName) { append(uvcoordinates, accessor, data, stride); texcoordBufferInfo = { .count = accessor.count, .stride = stride, .byteSize = totalSize}; }
                }

                auto const &[accessor, bufferView, buffer, data, stride, totalSize] = __getData(document, primitive.indices);
                append(indices, accessor, data, stride);
                indexBufferInfo = { .count = accessor.count, .stride = stride, .byteSize = totalSize};
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

        engine::writeFile(outputAttributeBufferPathAbs, attributeBuffer);
        engine::writeFile(outputIndexBufferPathAbs,     indexBuffer);

        mesh::SMeshDataFile dataFile {};
        dataFile.uid                 = 1234;
        dataFile.name                = meshID;
        dataFile.dataBinaryFilename  = outputAttributeBufferPath;
        dataFile.indexBinaryFilename = outputIndexBufferPath;

        mesh::SMeshAttributeDescription positionAttribute {};
        mesh::SMeshAttributeDescription normalAttribute   {};
        mesh::SMeshAttributeDescription tangentAttribute  {};
        mesh::SMeshAttributeDescription texcoordAttribute {};
        mesh::SMeshAttributeDescription indexAttribute    {};

        positionAttribute.name           = "POSITION";
        positionAttribute.index          = 0;
        positionAttribute.offset         = 0;
        positionAttribute.length         = positionBufferInfo.count;
        positionAttribute.bytesPerSample = positionBufferInfo.stride;

        normalAttribute.name             = "NORMAL";
        normalAttribute.index            = 0;
        normalAttribute.offset           = (positionAttribute.offset + positionAttribute.length);
        normalAttribute.length           = normalBufferInfo.count;
        normalAttribute.bytesPerSample   = normalBufferInfo.stride;

        tangentAttribute.name           = "TANGENT";
        tangentAttribute.index          = 0;
        tangentAttribute.offset         = (normalAttribute.offset + normalAttribute.length);
        tangentAttribute.length         = tangentBufferInfo.count;
        tangentAttribute.bytesPerSample = tangentBufferInfo.stride;

        texcoordAttribute.name           = "TEXCOORD";
        texcoordAttribute.index          = 0;
        texcoordAttribute.offset         = (tangentAttribute.offset + tangentAttribute.length);
        texcoordAttribute.length         = texcoordBufferInfo.count;
        texcoordAttribute.bytesPerSample = texcoordBufferInfo.stride;

        dataFile.attributes.push_back(positionAttribute);
        dataFile.attributes.push_back(normalAttribute);
        dataFile.attributes.push_back(tangentAttribute);
        dataFile.attributes.push_back(texcoordAttribute);

        indexAttribute.name           = "Indices";
        indexAttribute.index          = 0;
        indexAttribute.offset         = 0;
        indexAttribute.length         = indexBufferInfo.count;
        indexAttribute.bytesPerSample = indexBufferInfo.stride;

        dataFile.indices = indexAttribute;

        std::string serializedData = {};

        // Write datafile
        CResult<EResult> const dataFileSerializationResult = serializeMeshDataFile(dataFile, serializedData);
        if(not dataFileSerializationResult.successful())
        {
            CLog::Error(logTag(), "Failed to serialize data file.");
            return EResult::SerializationFailed;
        }

        engine::writeFile(outputDataFilePathAbs, serializedData);

        engine::mesh::SMeshMeta meta {};
        meta.uid        = 1234;
        meta.name       = meshID;
        meta.dataFileId = engine::util::crc32FromString(outputDataFilePath);

        // Write meta
        CResult<EResult> const metaSerializationResult = serializeMeshMeta(meta, serializedData);
        if(not metaSerializationResult.successful())
        {
            CLog::Error(logTag(), "Failed to serialize meta data.");
            return EResult::SerializationFailed;
        }

        engine::writeFile(outputMetaFilePathAbs, serializedData);

        return EResult::Success;
    }
}
