#include <iostream>

#include "SEPUtil/Documents/JSON.h"

#include "SEPCore/ObjectModel/SR_SEPCore_Object.h"
#include "SEPCore/ObjectModel/SR_SEPCore_Property.h"
#include "SEPCore/ObjectModel/Serialization/SR_SEPCore_JSONObjectSerializer.h"

namespace sr {
  namespace serialization {
    using namespace sr::documents;

    /**********************************************************************************************//**
     * \fn  template <typename TCollection> static bool serializeCollection( Ptr<IObjectSerializer> &serializer, std::string const&name, TCollection const&collection)
     *
     * \brief Serialize collection
     *
     * \tparam  TCollection Type of the collection.
     * \param [in,out]  serializer  The serializer.
     * \param           name        The name.
     * \param           collection  The collection.
     *
     * \return  True if it succeeds, false if it fails.
     **************************************************************************************************/
    template <typename TCollection>
    static bool serializeCollection(
      Ptr<IObjectSerializer>      &serializer,
      std::string            const&name,
      TCollection            const&collection)
    {
      for(typename TCollection::value_type const&item : collection) {
        item.second->acceptSerializer(serializer);
      }

      return true;
    }

    /**********************************************************************************************//**
     * \fn  bool XMLSerializer::serializeObject(Ptr<Object> const&object)
     *
     * \brief Serialize object
     *
     * \param object  The object.
     *
     * \return  True if it succeeds, false if it fails.
     **************************************************************************************************/
    bool JSONSerializer::serializeObject(Ptr<Object> const&object) {
      Ptr<IObjectSerializer> serializer = SharedFromThis<IObjectSerializer>(this);

      //
      // Serialize an object at this point, applying the subsequent format:
      // 

      // Serialize all it's properties!
      Object::Properties const&properties = object->properties();
      if(!serializeCollection<Object::Properties>(serializer, "properties", properties)) {
        // Log
        return false;
      }

      // Serialize all children
      Object::Children const&children = object->children();
      if(!serializeCollection<Object::Children>(serializer, "children", children)) {
        // Log 
        return false;
      }

      return true;
    }

    /**********************************************************************************************//**
     * \fn  bool XMLSerializer::serializeProperty(Ptr<Object> const&property)
     *
     * \brief Serialize property
     *
     * \param property  The property.
     *
     * \return  True if it succeeds, false if it fails.
     **************************************************************************************************/
    bool JSONSerializer::serializeProperty(Ptr<Object> const&property) {
      return true;
    }

    /**********************************************************************************************//**
     * \fn  bool XMLSerializer::deserializeObject(Ptr<Object> &object)
     *
     * \brief Deserialize object/
     *
     * \param [in,out]  object  The object.
     *
     * \return  True if it succeeds, false if it fails.
     **************************************************************************************************/
    bool JSONSerializer::deserializeObject(Ptr<Object> &object) {
      return true;
    }

    /**********************************************************************************************//**
     * \fn  bool XMLSerializer::deserializeProperty(Ptr<Object> &property)
     *
     * \brief Deserialize property
     *
     * \param [in,out]  property  The property.
     *
     * \return  True if it succeeds, false if it fails.
     **************************************************************************************************/
    bool JSONSerializer::deserializeProperty(Ptr<Object> &property) {
      return true;
    }

    /**********************************************************************************************//**
     * \fn  std::string XMLSerializer::serializeResultToString()
     *
     * \brief Serialize result to string
     *
     * \return  A std::string.
     **************************************************************************************************/
    std::string JSONSerializer::serializeResultToString() {
      return "";
    }

  }
}
