#include "Renderer/FrameGraph/Pass.h"

namespace engine {
  namespace framegraph {


    PassBase::Accessor::Accessor(PassBase const*pass)
      : m_pass(pass)
    {}

    FrameGraphResourceIdList const&
      PassBase::Accessor::resourceReferences() const
    {
      return m_pass->m_resourceReferences;
    }

    PassBase::MutableAccessor::MutableAccessor(PassBase *pass)
      : Accessor(pass)
      , m_pass(pass)
    {}

    FrameGraphResourceIdList&
      PassBase::MutableAccessor::mutableResourceReferences()
    {
      return m_pass->m_resourceReferences;
    }

    bool
      PassBase::MutableAccessor::registerResource(FrameGraphResourceId_t const&id)
    {
      return m_pass->registerResource(id);
    }

    UniqueCStdSharedPtr_t<PassBase::Accessor>
      PassBase::getAccessor(PassKey<GraphBuilder>&&) const
    {
      return std::move(std::make_unique<PassBase::Accessor>(this));
    }

    UniqueCStdSharedPtr_t<PassBase::MutableAccessor>
      PassBase::getMutableAccessor(PassKey<GraphBuilder>&&)
    {
      return std::move(std::make_unique<PassBase::MutableAccessor>(this));
    }

    UniqueCStdSharedPtr_t<PassBase::Accessor>
      PassBase::getAccessor(PassKey<PassBuilder>&&) const
    {
      return std::move(std::make_unique<PassBase::Accessor>(this));
    }

    UniqueCStdSharedPtr_t<PassBase::MutableAccessor>
      PassBase::getMutableAccessor(PassKey<PassBuilder>&&)
    {
      return std::move(std::make_unique<PassBase::MutableAccessor>(this));
    }

    UniqueCStdSharedPtr_t<PassBase::Accessor>
      PassBase::getAccessor(PassKey<Graph>&&) const
    {
      return std::move(std::make_unique<PassBase::Accessor>(this));
    }

    PassBase::PassBase(
      PassUID_t   const&passUID,
      std::string const&passName)
      : m_passUID(passUID)
      , m_passName(passName)
    {}

    bool
      PassBase::setup(PassBuilder&)
    {
      return true;
    }
    bool
      PassBase::execute(
        FrameGraphResources           const&frameGraphResources,
        CStdSharedPtr_t<IFrameGraphRenderContext>      &pass)
    {
      return true;
    }

    std::string const&
      PassBase::passName() const
    {
      return m_passName;
    }
    PassUID_t const&
      PassBase::passUID() const
    {
      return m_passUID;
    }

    void
      PassBase::acceptSerializer(CStdSharedPtr_t<IFrameGraphSerializer> s)
    {
      s->serializePass(*this);
    }

    void
      PassBase::acceptDeserializer(CStdSharedPtr_t<IFrameGraphDeserializer> const&d)
    {
      d->deserializePass(*this);
    }

    bool
      PassBase::registerResource(FrameGraphResourceId_t const&id)
    {
      m_resourceReferences.push_back(id);
      return true;
    }

  }
}