#include "textures/textureprocessor.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <core/databuffer.h>
#include <core/result.h>
#include <core/helpers.h>
#include <core/enginetypehelper.h>
#include <util/documents/json.h>
#include <util/crc32.h>
#include <textures/declaration.h>

#include "common/functions.h"

//
// Created by dotti on 09.12.19.
//
namespace texture
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
    static CResult<EResult> serializeTextureMeta(textures::STextureMeta const &aMeta, std::string &aOutSerializedData)
    {
        using namespace resource_compiler::serialization;
        using namespace engine::documents;

        Unique<IJSONSerializer<textures::STextureMeta>> serializer  = makeUnique<CJSONSerializer<textures::STextureMeta>>();
        bool const                                      initialized = serializer->initialize();
        if( not initialized )
        {
            return EResult::SerializationFailed;
        }
        CResult<Shared<serialization::ISerializer<textures::STextureMeta>::IResult>> const serialization = serializer->serialize(aMeta);
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

    struct STextureLoadInfo
    {
        bool                success;
        asset::STextureInfo meta;
        ByteBuffer          data;
    };

    struct STextureCollectionLoadInfo
    {
        bool                     success;
        asset::STextureInfo      meta;
        Vector<STextureLoadInfo> inputs;
    };

    STextureLoadInfo __loadTextureFromFile(std::filesystem::path const &aFilename)
    {
        STextureLoadInfo loadInfo {};
        loadInfo.success = false;

        try
        {
            std::string const fn = aFilename.string();

            bool is16bit = stbi_is_16_bit(fn.c_str());
            bool isHDR   = stbi_is_hdr(fn.c_str());

            int   w   = 0
            , h   = 0
            , c   = 0;

            if(not (is16bit || isHDR))
            {
                stbi_uc* stbuc = stbi_load(fn.c_str(), &w, &h, &c, 4);
                if(nullptr == stbuc)
                {
                    char const *reason = stbi_failure_reason();
                    CLog::Error(logTag(), "Failed to load image w/ name {}. Reason: {}", aFilename.string(), reason);
                    return loadInfo;
                }

                loadInfo.meta.width          = w;
                loadInfo.meta.height         = h;
                loadInfo.meta.channels       = c;
                loadInfo.meta.bitsPerChannel = 8;
                loadInfo.meta.depth          = 1;
                loadInfo.meta.arraySize      = 1;
                loadInfo.meta.mipLevels      = 1;
                loadInfo.meta.format         = asset::EFormat::R8G8B8A8_UNORM;

                std::vector<uint8_t> data {};
                data.resize((w * h * c * sizeof(uint8_t)));
                memcpy(data.data(), stbuc, data.size());

                std::size_t const size = data.size();
                loadInfo.data = ByteBuffer(std::move(data), size);

                stbi_image_free(stbuc);
                loadInfo.success = true;
            }
            else if(is16bit)
            {
                stbi_us* stbuc = stbi_load_16(fn.c_str(), &w, &h, &c, 4);
                if(nullptr == stbuc)
                {
                    char const *reason = stbi_failure_reason();
                    CLog::Error(logTag(), "Failed to load image w/ name {}. Reason: {}", aFilename.string(), reason);
                    loadInfo.success = false;
                    return loadInfo;
                }

                loadInfo.meta.width          = w;
                loadInfo.meta.height         = h;
                loadInfo.meta.channels       = c;
                loadInfo.meta.bitsPerChannel = 16;
                loadInfo.meta.depth          = 1;
                loadInfo.meta.arraySize      = 1;
                loadInfo.meta.mipLevels      = 1;
                loadInfo.meta.format         = asset::EFormat::R16G16B16A16_UNORM;

                std::vector<uint8_t> data {};
                data.resize((w * h * c * sizeof(uint16_t)));
                memcpy(data.data(), stbuc, data.size());

                std::size_t const size = data.size();
                loadInfo.data = ByteBuffer(std::move(data), size);

                stbi_image_free(stbuc);
                loadInfo.success = true;
            }
            else
            {
                float* stbuc = stbi_loadf(fn.c_str(), &w, &h, &c, 4);
                if(nullptr == stbuc)
                {
                    char const *reason = stbi_failure_reason();
                    CLog::Error(logTag(), "Failed to load image w/ name {}. Reason: {}", aFilename.string(), reason);
                    loadInfo.success = false;
                    return loadInfo;
                }

                loadInfo.meta.width          = w;
                loadInfo.meta.height         = h;
                loadInfo.meta.channels       = c;
                loadInfo.meta.bitsPerChannel = 32;
                loadInfo.meta.depth          = 1;
                loadInfo.meta.arraySize      = 1;
                loadInfo.meta.mipLevels      = 1;
                loadInfo.meta.format         = asset::EFormat::R32G32B32A32_FLOAT;

                std::vector<uint8_t> data {};
                data.resize((w * h * c * sizeof(float)));
                memcpy(data.data(), stbuc, data.size());

                std::size_t const size = data.size();
                loadInfo.data = ByteBuffer(std::move(data), size);

                stbi_image_free(stbuc);
                loadInfo.success = true;
            }

            return loadInfo;
        } catch(...)
        {
            return loadInfo;
        }
    }

    STextureCollectionLoadInfo __loadTexturesFromFiles(std::vector<std::filesystem::path> const &aFilenames) {
        STextureCollectionLoadInfo infos{};
        infos.inputs.resize(aFilenames.size());

        for (std::size_t k = 0; k < aFilenames.size(); ++k) {
            std::filesystem::path const &fn = aFilenames.at(k);
            infos.inputs[k] = __loadTextureFromFile(fn);
        }

        // Post processing.

        // Step 1
        // The sizes, component counts and bits per channel are required to match!
        bool totalMatch = true;
        if (1 < infos.inputs.size())
        {
            STextureLoadInfo const &previous = *infos.inputs.begin();
            for(std::size_t k=0; k<infos.inputs.size(); ++k)
            {
                if(0 == k) continue;

                STextureLoadInfo const &current = infos.inputs.at(k);

                bool const match = (previous.meta.format         == current.meta.format         &&
                                    previous.meta.width          == current.meta.width          &&
                                    previous.meta.height         == current.meta.height         &&
                                    previous.meta.depth          == current.meta.depth          &&
                                    previous.meta.arraySize      == current.meta.arraySize      &&
                                    previous.meta.mipLevels      == current.meta.mipLevels      &&
                                    previous.meta.channels       == current.meta.channels       &&
                                    previous.meta.bitsPerChannel == current.meta.bitsPerChannel);
                if(not (totalMatch &= match))
                {
                    break;
                }
            }
        }

        if(not totalMatch)
        {
            infos.success = false;
        }
        else
        {
            infos.meta    = infos.inputs.begin()->meta; // Thank you, constraints...
            infos.success = true;
        }

        return infos;
    }

    CResult<EResult> processTexture(std::filesystem::path const &aTextureFile, SConfiguration const &aConfig)
    {
        std::filesystem::path const &pathAbs    = std::filesystem::current_path() / aTextureFile;
        std::filesystem::path const &parentPath = std::filesystem::relative(aTextureFile, aConfig.inputPath).parent_path();
        std::filesystem::path const &textureID  = aTextureFile.stem();

        std::filesystem::path const outputPath                      = ( parentPath)                                                                      .lexically_normal();
        std::filesystem::path const outputMetaFilePath              = ( parentPath / (std::filesystem::path(textureID.string() + ".texture.meta"))  ).lexically_normal();
        std::filesystem::path const outputDataFilePath              = ( parentPath / (std::filesystem::path(textureID.string() + ".texturedata"))).lexically_normal();
        std::filesystem::path const outputPathAbsolute              = (std::filesystem::current_path() / aConfig.outputPath / outputPath                 ).lexically_normal();
        std::filesystem::path const outputMetaFilePathAbs           = (std::filesystem::current_path() / aConfig.outputPath / outputMetaFilePath         ).lexically_normal();
        std::filesystem::path const outputDataFilePathAbs           = (std::filesystem::current_path() / aConfig.outputPath / outputDataFilePath         ).lexically_normal();

        resource_compiler::checkPathExists(outputPathAbsolute);

        std::string const indexFileContents = readFile(aTextureFile);

        Shared<documents::IJSONDeserializer<textures::STextureFile>> indexDeserializer = makeShared<documents::CJSONDeserializer<textures::STextureFile>>();
        indexDeserializer->initialize();
        auto [success, index] = indexDeserializer->deserialize(indexFileContents);
        if(not success)
        {
            CLog::Error(logTag(), "Could not deserialize texture index file.");
            return false;
        }
        indexDeserializer->deinitialize();

        textures::STextureFile const indexData = *static_cast<textures::STextureFile const *>(&(index->asT().data()));

        Vector<std::filesystem::path> textureInputPaths = indexData.textureSourceFilenames;
        Vector<std::filesystem::path> enriched {};
        for(auto const &path : textureInputPaths)
        {
            enriched.push_back(pathAbs.parent_path() / path);
        }

        STextureCollectionLoadInfo textureInputLoads = __loadTexturesFromFiles(enriched);

        // Write out joined texture array...
        uint64_t   totalTextureDataSize = (textureInputLoads.inputs[0].data.size() * textureInputLoads.inputs.size()); // Thank you again, constraints...
        ByteBuffer buffer               = ByteBuffer::DataArrayFromSize(totalTextureDataSize);
        for(std::size_t k=0; k<textureInputLoads.inputs.size(); ++k)
        {
            STextureLoadInfo const &input = textureInputLoads.inputs.at(k);
            memcpy(buffer.mutableDataVector().data() + (k * input.data.size()), input.data.dataVector().data(), input.data.size());
        }

        engine::writeFile(outputDataFilePathAbs, buffer.dataVector());

        std::string serializedData = {};

        // Write meta
        textures::STextureMeta meta {};
        meta.uid                  = indexData.uid;
        meta.name                 = indexData.name;
        meta.textureInfo          = textureInputLoads.meta;
        meta.imageLayersBinaryUid = util::crc32FromString(outputDataFilePath.string());

        CResult<EResult> const metaSerializationResult = serializeTextureMeta(meta, serializedData);
        if(not metaSerializationResult.successful())
        {
            CLog::Error(logTag(), "Failed to serialize meta data.");
            return EResult::SerializationFailed;
        }

        engine::writeFile(outputMetaFilePathAbs, serializedData);

        return EResult::Success;
    }
}
