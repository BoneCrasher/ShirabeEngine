#ifndef __SHIRABE_FRAMEGRAPH_FRAMEGRAPH_H__
#define __SHIRABE_FRAMEGRAPH_FRAMEGRAPH_H__

#include <string>
#include <functional>
#include <stack>

#include "Core/EngineTypeHelper.h"
#include "Core/PassKey.h"
#include "Log/Log.h"
#include "Resources/Core/ResourceDTO.h"

#include "Renderer/FrameGraph/FrameGraphSerialization.h"
#include "Renderer/FrameGraph/Pass.h"
#include "Renderer/FrameGraph/FrameGraphData.h"
#include "Renderer/FrameGraph/FrameGraphRenderContext.h"

namespace Engine {
  namespace FrameGraph {

    class SHIRABE_TEST_EXPORT Graph
      : public ISerializable<IFrameGraphSerializer, IFrameGraphDeserializer>
    {
      DeclareLogTag(Graph);

    public:
      class SHIRABE_TEST_EXPORT Accessor {
      public:
        Accessor(Graph const*graph);

        AdjacencyListMap<PassUID_t>                         const&passAdjacency()           const;
        std::stack<PassUID_t>                               const&passExecutionOrder()      const;
        FrameGraphResourceIdList                            const&resources()               const;
        FrameGraphMutableResources                          const&resourceData()            const;
        AdjacencyListMap<FrameGraphResourceId_t>            const&resourceAdjacency()       const;
        std::stack<FrameGraphResourceId_t>                  const&resourceOrder()           const;
        AdjacencyListMap<PassUID_t, FrameGraphResourceId_t> const&passToResourceAdjacency() const;

      private:
        Graph const*m_graph;
      };
      
      class SHIRABE_TEST_EXPORT MutableAccessor
        : public Accessor
      {
      public:
        MutableAccessor(Graph *graph);
        
        AdjacencyListMap<PassUID_t>                         &mutablePassAdjacency();
        std::stack<PassUID_t>                               &mutablePassExecutionOrder();
        FrameGraphResourceIdList                            &mutableResources();
        FrameGraphMutableResources                          &mutableResourceData();
        AdjacencyListMap<FrameGraphResourceId_t>            &mutableResourceAdjacency();
        std::stack<FrameGraphResourceId_t>                  &mutableResourceOrder();
        AdjacencyListMap<PassUID_t, FrameGraphResourceId_t> &mutablePassToResourceAdjacency();

        template <typename TPass, typename... TPassCreationArgs>
        Ptr<TPass> createPass(
          PassUID_t         const&uid,
          std::string       const&name,
          TPassCreationArgs &&... args);

      private:
        Graph *m_graph;
      };

      UniquePtr<Accessor> getAccessor(PassKey<class GraphBuilder> &&key) const {
        return std::move(std::make_unique<Accessor>(this));
      }

      UniquePtr<MutableAccessor> getMutableAccessor(PassKey<class GraphBuilder> &&key) {
        return std::move(std::make_unique<MutableAccessor>(this));
      }

      UniquePtr<Accessor> getAccessor(PassKey<FrameGraphGraphVizSerializer> &&key) const {
        return std::move(std::make_unique<Accessor>(this));
      }
      
      UniquePtr<MutableAccessor> getMutableAccessor(PassKey<FrameGraphGraphVizSerializer> &&key) {
        return std::move(std::make_unique<MutableAccessor>(this));
      }

      bool
        execute(Ptr<IFrameGraphRenderContext>&);

      virtual
        void acceptSerializer(Ptr<IFrameGraphSerializer> s);

      virtual
        void acceptDeserializer(Ptr<IFrameGraphDeserializer> const&d);

      inline
        Graph& operator=(Graph const&other);

      PassMap const&passes() const;

    private:

      bool initializeResources(
        Ptr<IFrameGraphRenderContext>       renderContext,
        FrameGraphResourceIdList      const&resourceIds);

      bool bindResources(
        Ptr<IFrameGraphRenderContext>       renderContext,
        FrameGraphResourceIdList      const&resourceIds);

      bool unbindResources(
        Ptr<IFrameGraphRenderContext>       renderContext,
        FrameGraphResourceIdList      const&resourceIds);

      bool deinitializeResources(
        Ptr<IFrameGraphRenderContext>       renderContext,
        FrameGraphResourceIdList      const&resourceIds);

      bool initializeTexture(
        Ptr<IFrameGraphRenderContext> renderContext,
        Ptr<FrameGraphTexture>        texture);
      bool initializeTextureView(
        Ptr<IFrameGraphRenderContext> renderContext,
        Ptr<FrameGraphTexture>        texture,
        Ptr<FrameGraphTextureView>    textureView);
      bool initializeBuffer(
        Ptr<IFrameGraphRenderContext> renderContext);
      bool initializeBufferView(
        Ptr<IFrameGraphRenderContext> renderContext);
      
      bool deinitializeTexture(
        Ptr<IFrameGraphRenderContext> renderContext,
        Ptr<FrameGraphTexture>        texture);
      bool deinitializeTextureView(
        Ptr<IFrameGraphRenderContext> renderContext,
        Ptr<FrameGraphTexture>        texture,
        Ptr<FrameGraphTextureView>    textureView);
      bool deinitializeBufferView(
        Ptr<IFrameGraphRenderContext> renderContext);
      bool deinitializeBuffer(
        Ptr<IFrameGraphRenderContext> renderContext);

      bool addPass(Ptr<PassBase> const&);

      template <typename TPass, typename... TPassCreationArgs>
      Ptr<TPass> createPass(
        PassUID_t         const&uid,
        std::string       const&name,
        TPassCreationArgs &&... args);

      // 
      Ptr<IResourceManager> m_resourceManager;

      PassMap                     m_passes;
      AdjacencyListMap<PassUID_t> m_passAdjacency;
      std::stack<PassUID_t>       m_passExecutionOrder;

      FrameGraphResourceIdList   m_resources;
      FrameGraphMutableResources m_resourceData;

      AdjacencyListMap<FrameGraphResourceId_t> m_resourceAdjacency;
      std::stack<FrameGraphResourceId_t>       m_resourceOrder;

      AdjacencyListMap<PassUID_t, FrameGraphResourceId_t> m_passToResourceAdjacency;

      FrameGraphResourceIdList m_instantiatedResources;
    };
    
    template <typename TPass, typename... TPassCreationArgs>
    Ptr<TPass>
      Graph::MutableAccessor::createPass(
        PassUID_t         const&uid,
        std::string       const&name,
        TPassCreationArgs &&... args)
    {
      return m_graph->createPass<TPass, TPassCreationArgs...>(uid, name, std::forward<TPassCreationArgs>(args)...);
    }

    template <typename TPass, typename... TPassCreationArgs>
    Ptr<TPass>
      Graph::createPass(
        PassUID_t         const&uid, 
        std::string       const&name, 
        TPassCreationArgs &&... args)
    {
      Ptr<TPass> pass = MakeSharedPointerType<TPass>(uid, name, std::forward<TPassCreationArgs>(args)...);
      if(!pass) {
        //...
      }

      this->addPass(pass);
      return pass;
    }
  }
}

#endif