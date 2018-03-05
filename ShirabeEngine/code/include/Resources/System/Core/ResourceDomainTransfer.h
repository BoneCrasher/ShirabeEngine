#ifndef __SHIRABE_IRESOURCEDESCRIPTOR_H__
#define __SHIRABE_IRESOURCEDESCRIPTOR_H__

#include <iostream>
#include <sstream>
#include <string>

#include "Core/EngineTypeHelper.h"
#include "Resources/System/Core/EResourceType.h"
#include "Resources/System/Core/ResourceDTO.h"

namespace Engine {
  namespace Resources {
    
    /**********************************************************************************************//**
     * \class DescriptorImplBase
     *
     * \brief A descriptor implementation base.
     *
     * \tparam  TResource Type of the resource.
     **************************************************************************************************/
    template <EResourceType resource_type, EResourceSubType resource_subtype>
    class DescriptorImplBase {
      static const constexpr EResourceType    resource_type    = resource_type;
      static const constexpr EResourceSubType resource_subtype = resource_subtype;
      
    public:
      EResourceType    type()    const { return resource_type;    }
      EResourceSubType subtype() const { return resource_subtype; }   

      std::string          name;
      PublicResourceIdList dependencies;

      virtual std::string toString() const {
        std::stringstream ss;
        ss
          << "DescriptorImplementationBase<" << resource_type << ", " << resource_subtype << "> {\n"
          << "  Name: " << name << "\n"
          << "  Dependencies: \n";
        for(PublicResourceId_t const&id : dependencies)
          ss << id << ", \n";

        return ss.str();
      }
    };

    // An adapter class will hold the implementation of a DomainTransferObject, e.g.
    // the descriptor_impl_type of ResourceDescriptor<TextureND<1>>.
    // Since the ResourceDescriptor<T> specializiation will derive from the impl. type
    // slicing at this point won't be an issue. 
    // The only thing of matter could be static downcasting and public Interface issues 
    // with knowing the capabilities of the underlying resource class.

#define DeclareAdapter(type, alias, name)                    \
		template <typename Impl>                                 \
		class type##Adapter {								                     \
		public:												                           \
			typedef typename Impl alias##_impl;                    \
															                               \
			inline type##Adapter(							                     \
				const Impl& name)						                         \
			  : _##name(name)					 			                       \
			{}												                             \
															                               \
			inline const Impl        				                       \
				name() const { return static_cast<Impl>(_##name); }  \
															                               \
    private:											                           \
      alias##_impl _##name;									                 \
		};			

    DeclareAdapter(ResourceDescriptor,
                   descriptor_type,
                   descriptor);

    DeclareAdapter(ResourceBinding, 
                   binding_type, 
                   binding);

    DeclareAdapter(ResourceCreationRequest,
                   creation_request_type, 
                   creationRequest);

    DeclareAdapter(ResourceUpdateRequest,
                   update_request_type,
                   updateRequest);

    DeclareAdapter(ResourceQueryRequest, 
                   query_request_type, 
                   queryRequest);

    DeclareAdapter(ResourceDestructionRequest,
                   destruction_request_type, 
                   destructionRequest);

    template <typename TResource>
    static inline std::ostream& operator <<(std::ostream& s, typename TResource::Binding const& d) {
      return (s << d.toString());
    }

    template <typename TResource>
    static inline std::ostream& operator <<(std::ostream& s, typename TResource::Descriptor const& d) {
      return (s << d.toString());
    }

    template <typename TResource>
    static inline std::ostream& operator <<(std::ostream& s, typename TResource::CreationRequest const& d) {
      return (s << d.toString());
    }

    template <typename TResource>
    static inline std::ostream& operator <<(std::ostream& s, typename TResource::UpdateRequest const& d) {
      return (s << d.toString());
    }

    template <typename TResource>
    static inline std::ostream& operator <<(std::ostream& s, typename TResource::Query const& d) {
      return (s << d.toString());
    }

    template <typename TResource>
    static inline std::ostream& operator <<(std::ostream& s, typename TResource::DestructionRequest const& d) {
      return (s << d.toString());
    }
  }
}

#endif