#include "materialsystem/declaration.h"

#include <util/documents/json.h>


namespace engine::material
{

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    EEngineStatus CSharedMaterial::initializeMemory(Shared<memory::allocators::CAllocator> aAllocator)
    {
        if(nullptr == aAllocator)
        {
            return EEngineStatus::Error;
        }

        // Read size requirements


        // Allocate data

        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    EEngineStatus CMaterialInstance::createConfiguration(CSharedMaterial const &aMaterial, bool aIncludeSystemBuffers)
    {
        if(mConfiguration.has_value())
        {
            return EEngineStatus::Ok;
        }

        Shared<CSharedMaterial> m = sharedMaterial();
        m->

        CMaterialConfig config = CMaterialConfig::fromMaterialDesc(aMaterial, aIncludeSystemBuffers);

        mConfiguration = std::move(config);

        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CMaterialConfig CMaterialConfig::fromMaterialDesc(CSharedMaterial const &aMaterial, bool aIncludeSystemBuffers)
    {
        // uint32_t const minUBOOffsetAlignment = 0x100; // Hardcoded for the platform SCHLACHTSCHIFF ... make accessible in any other way...
        uint32_t const minUBOOffsetAlignment = 256; // 0x20;  // Hardcoded for the platform LENOVO ... make accessible in any other way...
        std::size_t alignment = minUBOOffsetAlignment; // 16 * sizeof(float);
        // if (0 < minUBOOffsetAlignment) {
        //     alignment = (alignment + minUBOOffsetAlignment - 1) & ~(minUBOOffsetAlignment - 1);
        // }

        auto const nextMultiple = [] (uint32_t const &aInput, uint32_t const &aAlignment) -> uint32_t
        {
            return (aInput + aAlignment - 1) & -aAlignment;
        };

        std::function<void(CMaterialConfig::BufferValueIndex_t &
                         , Shared<SBufferMember> &
                         , std::string const &
                         , uint64_t const &
                         , uint64_t const &)> iterate = nullptr;

        iterate = [&iterate, &nextMultiple, &alignment] (
                              CMaterialConfig::BufferValueIndex_t       &aIndex
                            , Shared<SBufferMember>                     &aMember
                            , std::string                         const &aParentPath
                            , uint64_t                            const &aOffsetBackshift
                            , uint64_t                            const &aCurrentBaseOffset)
        {
            std::string const path = fmt::format("{}.{}", aParentPath, aMember->name);
            aIndex.insert({path, aMember});

            uint64_t const arrayLayers = std::max(1lu, aMember->array.layers); // Ensure at least one iteration with std::max...
            for(std::size_t k=0; k<arrayLayers; ++k)
            {
                Shared<SBufferMember> member = makeShared<SBufferMember>(*aMember);

                member->location.offset  = (aCurrentBaseOffset + member->location.offset + (k * member->array.stride));
                member->location.length  = member->location.length; // nextMultiple(aMember->location.length, alignment);

                std::string prefixPath = path;
                if(1 < arrayLayers)
                {
                    prefixPath = fmt::format("{}[{}]", path, k);
                }
                aIndex.insert({prefixPath, member});

                for(auto &[n, v] : member->members)
                {
                    iterate(aIndex, v, prefixPath, aOffsetBackshift, member->location.offset);
                }
            }
        };

        CMaterialConfig config {};

        if(aMaterial.uniformBuffers().empty())
        {
            return {}; // Nothing to do...
        }

        std::vector<SUniformBuffer> sorted(aMaterial.uniformBuffers());

        //
        // Filter out all non-user-set indexed buffers, so that they won't have any influence on the buffer size calculation.
        //
        bool const processSystemUBOs = aIncludeSystemBuffers;
        if(not processSystemUBOs)
        {
            static constexpr uint8_t sFirstPermittedUserSetIndex = 2;

            auto const filter = [](SUniformBuffer const &aBuffer) -> bool
            {
                return (sFirstPermittedUserSetIndex > aBuffer.set);
            };

            sorted.erase(
                    std::remove_if(sorted.begin(), sorted.end(), filter)
                    , sorted.end());
        }

        //
        // Sort by set, then binding, so that we properly calculate the buffer sizes and offsets...
        //
        auto const sort = [] (SUniformBuffer const &aLhs, SUniformBuffer const &aRhs) -> bool
        {
            return (aLhs.set < aRhs.set)                                      // First by set
                    || (aLhs.set == aRhs.set && aLhs.binding < aRhs.binding); // Then by binding;
        };
        std::sort(sorted.begin(), sorted.end(), sort);

        //
        // Due to previous sorting and optional filtering by set and index, we can simply check the offset of the first buffer and subtract it from all the other offsets.
        //
        uint64_t const baseBackShift = sorted[0].location.offset;
        for(auto &buffer : sorted)
        {
            CMaterialConfig::BufferValueIndex_t bufferValueIndex {};

            Shared<SBufferMember> member = makeShared<SBufferMember>();
            member->name             = buffer.name;
            member->location.offset  = buffer.location.offset;
            member->location.length  = buffer.location.length; // nextMultiple(buffer.location.length, alignment);
            member->location.padding = buffer.location.padding;
            member->array            = buffer.array;
            member->members          = buffer.members;

            std::string const path = fmt::format("{}", buffer.name);
            bufferValueIndex.insert({path, member});

            uint64_t const arrayLayers = std::max(1lu, buffer.array.layers); // Ensure at least one iteration with std::max...
            for(std::size_t k=0; k<arrayLayers; ++k)
            {
                buffer.location.offset = (buffer.location.offset + (k * buffer.array.stride));

                Shared<SBufferMember> arrayMember = makeShared<SBufferMember>();
                arrayMember->name             = buffer.name;
                arrayMember->location.offset  = buffer.location.offset;
                arrayMember->location.length  = buffer.location.length; // nextMultiple(buffer.location.length, alignment);
                arrayMember->location.padding = buffer.location.padding;
                arrayMember->array.layers     = 1;
                arrayMember->array.stride     = buffer.array.stride;
                arrayMember->members          = buffer.members;

                std::string arrayPath = buffer.name;
                if(1 < arrayLayers)
                {
                    arrayPath = fmt::format("{}[{}]", buffer.name, k);
                }
                bufferValueIndex.insert({arrayPath, arrayMember});

                for(auto &[n, v] : buffer.members)
                {
                    iterate(bufferValueIndex, v, arrayPath, 0lu, buffer.location.offset);
                }
            }

            config.mBufferIndex.insert({ buffer.name, bufferValueIndex });

            int8_t *alignedData = (int8_t *)aligned_alloc(alignment, member->location.length);
            memset(alignedData, 0, member->location.length);
            config.mData.insert({ buffer.name, Shared<void>(alignedData) });
        }

        // config.mData.resize(totalSize);
        // config.mData.assign(totalSize, 0);

        return config;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    bool checkIsZeroArray(uint8_t const *aValue, SBufferLocation const &aLocation)
    {
        uint8_t chk = 0;
        for(uint64_t k=0; k<aLocation.length; ++k)
        {
            chk |= *(aValue + k);
        }

        return (0 == chk);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CEngineResult<void const *const> CMaterialConfig::getBuffer(std::string const &aBufferName) const
    {
        bool const has = hasBuffer(aBufferName);
        if(not has)
        {
            return CEngineResult<void const *const>(EEngineStatus::Error, nullptr);
        }

        Shared<void> const alignedData = mData.at(aBufferName);

        // SBufferData     const &buffer   = mBufferIndex.at(aBufferName);
        // SBufferLocation const &location = buffer.getLocation();
//
        // void const *const data = (mData.data() + location.offset);

        return { EEngineStatus::Ok, alignedData.get() };
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CEngineResult<void *const> CMaterialConfig::getBuffer(std::string const &aBufferName)
    {
        bool const has = hasBuffer(aBufferName);
        if(not has)
        {
            return CEngineResult<void *const>(EEngineStatus::Error, nullptr);
        }

        Shared<void> alignedData = mData.at(aBufferName);

        // SBufferData     const &buffer   = mBufferIndex.at(aBufferName);
        // SBufferLocation const &location = buffer.getLocation();

        // void *const data = (mData.data() + location.offset);

        return { EEngineStatus::Ok, alignedData.get() };
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    bool CMaterialConfig::acceptSerializer(documents::IJSONSerializer<CMaterialConfig> &aSerializer) const
    {
        aSerializer.beginObject("");

        // aSerializer.writeValue("uniformBufferData", mData);

        // aSerializer.beginArray("uniformBuffers");
        // auto const iterateUniformBuffers = [&] (BufferValueIndex_t::value_type const &aBuffer) -> void
        // {
        //     std::string           const &name = aBuffer.first;
        //     Shared<SBufferMember> const &data = aBuffer.second;
//
        //     SBufferLocation const &location = data->location;
        //     SBufferArray    const &array    = data->array;
//
        //     aSerializer.beginObject(name);
        //     aSerializer.writeValue("name",        name);
        //     aSerializer.writeValue("offset",      location.offset);
        //     aSerializer.writeValue("size",        location.length);
        //     aSerializer.writeValue("padding",     location.padding);
        //     aSerializer.writeValue("arraySize",   array.layers);
        //     aSerializer.writeValue("arrayStride", array.stride);
//
        //     aSerializer.beginArray("members");
        //     auto const iterate = [&] (BufferValueIndex_t::value_type const &aMember)
        //     {
        //         std::string           const  memberName = aMember.first;
        //         Shared<SBufferMember> const &memberData = aMember.second;
//
        //         SBufferLocation const &memberLocation = memberData->location;
        //         SBufferArray    const &memberArray    = memberData->array;
//
        //         aSerializer.beginObject(name);
        //         aSerializer.writeValue("name",        memberName);
        //         aSerializer.writeValue("offset",      memberLocation.offset);
        //         aSerializer.writeValue("size",        memberLocation.length);
        //         aSerializer.writeValue("padding",     memberLocation.padding);
        //         aSerializer.writeValue("arraySize",   memberArray.layers);
        //         aSerializer.writeValue("arrayStride", memberArray.stride);
        //         aSerializer.endObject();
        //     };
        //     std::for_each(data->members.begin(), data->members.end(), iterate);
        //     aSerializer.endArray();
//
        //     aSerializer.endObject();
        // };
        // std::for_each(mBufferIndex.begin(), mBufferIndex.end(), iterateUniformBuffers);
        // aSerializer.endArray();

        aSerializer.endObject();

        return true;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    bool CMaterialConfig::acceptDeserializer(documents::IJSONDeserializer<CMaterialConfig> &aDeserializer)
    {
        // aDeserializer.readValue("uniformBufferData", mData);

        uint32_t bufferCount = 0;
        aDeserializer.beginArray("uniformBuffers", bufferCount);
        //for(uint32_t k=0; k<bufferCount; ++k)
        //{
        //    SBufferData buffer = {};
//
        //    std::string     name     = {};
//
        //    aDeserializer.beginObject(k);
//
        //    aDeserializer.readValue("name",    name);
        //    aDeserializer.readValue("offset",  buffer.mLocation.offset);
        //    aDeserializer.readValue("size",    buffer.mLocation.length);
        //    aDeserializer.readValue("padding", buffer.mLocation.padding);
//
        //    uint32_t memberCount = 0;
        //    aDeserializer.beginArray("members", memberCount);
        //    for(uint32_t l=0; l<memberCount; ++l)
        //    {
        //        std::string     memberName ={};
        //        SBufferLocation location   ={};
//
        //        aDeserializer.beginObject(l);
//
        //        aDeserializer.readValue("name",    memberName);
        //        aDeserializer.readValue("offset",  location.offset);
        //        aDeserializer.readValue("size",    location.length);
        //        aDeserializer.readValue("padding", location.padding);
//
        //        aDeserializer.endObject();
//
        //        buffer.mValueIndex[memberName] = location;
        //    }
        //    aDeserializer.endArray();
//
        //    aDeserializer.endObject();
//
        //    mBufferIndex[name] = buffer;
        //}
        aDeserializer.endArray();

        return true;
    }
    //<-----------------------------------------------------------------------------
}
