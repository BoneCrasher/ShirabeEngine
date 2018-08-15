#ifndef __SHIRABE_FRAMEGRAPH_FRAMEGRAPH_H__
#define __SHIRABE_FRAMEGRAPH_FRAMEGRAPH_H__

#include <string>
#include <functional>
#include <stack>

#include "core/enginetypehelper.h"
#include "Core/PassKey.h"
#include "Log/Log.h"
#include "resources/core/resourcedto.h"

#include "Renderer/FrameGraph/FrameGraphSerialization.h"
#include "Renderer/FrameGraph/Pass.h"
#include "Renderer/FrameGraph/FrameGraphData.h"
#include "Renderer/FrameGraph/FrameGraphRenderContext.h"

namespace engine {
  namespace framegraph {

    class SHIRABE_TEST_EXPORT Graph
      : public ISerializable<IFrameGraphSerializer, IFrameGraphDeserializer>
    {
      SHIRABE_DECLARE_LOG_TAG(Graph);

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
        CStdSharedPtr_t<TPass> createPass(
          PassUID_t         const&uid,
          std::string       const&name,
          TPassCreationArgs &&... args);

      private:
        Graph *m_graph;
      };

      UniqueCStdSharedPtr_t<Accessor> getAccessor(PassKey<class GraphBuilder> &&key) const {
        return std::move(std::make_unique<Accessor>(this));
      }

      UniqueCStdSharedPtr_t<MutableAccessor> getMutableAccessor(PassKey<class GraphBuilder> &&key) {
        return std::move(std::make_unique<MutableAccessor>(this));
      }

      UniqueCStdSharedPtr_t<Accessor> getAccessor(PassKey<FrameGraphGraphVizSerializer> &&key) const {
        return std::move(std::make_unique<Accessor>(this));
      }
      
      UniqueCStdSharedPtr_t<MutableAccessor> getMutableAccessor(PassKey<FrameGraphGraphVizSerializer> &&key) {
        return std::move(std::make_unique<MutableAccessor>(this));
      }

      bool
        execute(CStdSharedPtr_t<IFrameGraphRenderContext>&);

      virtual
        void acceptSerializer(CStdSharedPtr_t<IFrameGraphSerializer> s);

      virtual
        void acceptDeserializer(CStdSharedPtr_t<IFrameGraphDeserializer> const&d);

      inline
        Graph& operator=(Graph const&other);

      PassMap const&passes() const;

    private:

      bool initializeResources(
        CStdSharedPtr_t<IFrameGraphRenderContext>       renderContext,
        FrameGraphResourceIdList      const&resourceIds);

      bool bindResources(
        CStdSharedPtr_t<IFrameGraphRenderContext>       renderContext,
        FrameGraphResourceIdList      const&resourceIds);

      bool unbindResources(
        CStdSharedPtr_t<IFrameGraphRenderContext>       renderContext,
        FrameGraphResourceIdList      const&resourceIds);

      bool deinitializeResources(
        CStdSharedPtr_t<IFrameGraphRenderContext>       renderContext,
        FrameGraphResourceIdList      const&resourceIds);

      bool initializeTexture(
        CStdSharedPtr_t<IFrameGraphRenderContext> renderContext,
        CStdSharedPtr_t<FrameGraphTexture>        texture);
      bool initializeTextureView(
        CStdSharedPtr_t<IFrameGraphRenderContext> renderContext,
        CStdSharedPtr_t<FrameGraphTexture>        texture,
        CStdSharedPtr_t<FrameGraphTextureView>    textureView);
      bool initializeBuffer(
        CStdSharedPtr_t<IFrameGraphRenderContext> renderContext);
      bool initializeBufferView(
        CStdSharedPtr_t<IFrameGraphRenderContext> renderContext);
      
      bool deinitializeTexture(
        CStdSharedPtr_t<IFrameGraphRenderContext> renderContext,
        CStdSharedPtr_t<FrameGraphTexture>        texture);
      bool deinitializeTextureView(
        CStdSharedPtr_t<IFrameGraphRenderContext> renderContext,
        CStdSharedPtr_t<FrameGraphTexture>        texture,
        CStdSharedPtr_t<FrameGraphTextureView>    textureView);
      bool deinitializeBufferView(
        CStdSharedPtr_t<IFrameGraphRenderContext> renderContext);
      bool deinitializeBuffer(
        CStdSharedPtr_t<IFrameGraphRenderContext> renderContext);

      bool addPass(CStdSharedPtr_t<PassBase> const&);

      template <typename TPass, typename... TPassCreationArgs>
      CStdSharedPtr_t<TPass> createPass(
        PassUID_t         const&uid,
        std::string       const&name,
        TPassCreationArgs &&... args);

      // 
      CStdSharedPtr_t<ResourceManager> m_resourceManager;

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
    CStdSharedPtr_t<TPass>
      Graph::MutableAccessor::createPass(
        PassUID_t         const&uid,
        std::string       const&name,
        TPassCreationArgs &&... args)
    {
      return m_graph->createPass<TPass, TPassCreationArgs...>(uid, name, std::forward<TPassCreationArgs>(args)...);
    }

    template <typename TPass, typename... TPassCreationArgs>
    CStdSharedPtr_t<TPass>
      Graph::createPass(
        PassUID_t         const&uid, 
        std::string       const&name, 
        TPassCreationArgs &&... args)
    {
      CStdSharedPtr_t<TPass> pass = makeCStdSharedPtr<TPass>(uid, name, std::forward<TPassCreationArgs>(args)...);
      if(!pass) {
        //...
      }

      this->addPass(pass);
      return pass;
    }
  }
}

#endif