#include "Renderer/FrameGraph/Passes/Lighting.h"

namespace Engine {
	namespace FrameGraph {


		LightingPass::LightingPass()  {

		}

		LightingPass::~LightingPass() {}

		bool 
			LightingPass::setup(
        Ptr<ApplicationEnvironment> const&environment,
        PassLinker<LightingPass>         &passLinker,
        InputData                        &input,
        OutputData                       &output,
        FrameGraphResource          const&gbuffer0,
        FrameGraphResource          const&gbuffer1,
        FrameGraphResource          const&gbuffer2,
        FrameGraphResource          const&gbuffer3)
		{      
			FrameGraphTexture lightAccBufferDesc ={ };
      lightAccBufferDesc.readableName = "Light Accumulation Buffer";
      lightAccBufferDesc.width        = 1920;
      lightAccBufferDesc.height       = 1080;
      lightAccBufferDesc.depth        = 1;
      lightAccBufferDesc.format       = FrameGraphFormat::R32_FLOAT;
      lightAccBufferDesc.mipLevels    = 1;
      lightAccBufferDesc.arraySize    = 1;
      lightAccBufferDesc.initialState = FrameGraphResourceInitState::Clear;

			m_state.lightAccumulationBufferTextureId = passLinker.createTexture(lightAccBufferDesc);
      
      FrameGraphReadTextureFlags readFlags{};
      readFlags.requiredFormat = FrameGraphFormat::Automatic;

      input.gbuffer0 = passLinker.readTexture(gbuffer0, readFlags, Range(0, 1), Range(0, 1));
      input.gbuffer1 = passLinker.readTexture(gbuffer1, readFlags, Range(0, 1), Range(0, 1));
      input.gbuffer2 = passLinker.readTexture(gbuffer2, readFlags, Range(0, 1), Range(0, 1));
      input.gbuffer3 = passLinker.readTexture(gbuffer3, readFlags, Range(0, 1), Range(0, 1));

      FrameGraphWriteTextureFlags writeFlags{ };
      writeFlags.requiredFormat = FrameGraphFormat::Automatic;
      writeFlags.writeTarget    = FrameGraphWriteTarget::Color;

			output.lightAccumulationBuffer = passLinker.writeTexture(output.lightAccumulationBuffer, writeFlags, Range(0, 1), Range(0, 1));

			return true;
		}

		bool 
			LightingPass::execute(
				Ptr<IRenderContext>     &context,
				InputData          const&input, 
				OutputData         const&output)
		{
			return true;
		}
	}
}