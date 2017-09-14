#include "pch.h"

#include <Kore/IO/FileReader.h>
#include <Kore/Graphics5/CommandList.h>
#include <Kore/Graphics5/Graphics.h>
#include <Kore/Graphics5/PipelineState.h>
#include <Kore/Graphics5/Shader.h>
#include <Kore/System.h>
#include <limits>
#include <stdlib.h>
#include <stdio.h>

using namespace Kore;

namespace {
	const int bufferCount = 2;
	int currentBuffer = 0;
	Graphics5::RenderTarget* framebuffers[bufferCount];
	Graphics5::Shader* vertexShader;
	Graphics5::Shader* fragmentShader;
	Graphics5::PipelineState* pipeline;
	Graphics5::VertexBuffer* vertices;
	Graphics5::IndexBuffer* indices;
	Graphics5::CommandList* commandList;

	void update() {
		//printf("update\n");
		currentBuffer = (currentBuffer + 1) % bufferCount;

		Graphics5::begin(framebuffers[currentBuffer]);

		commandList->begin();
		commandList->framebufferToRenderTargetBarrier(framebuffers[currentBuffer]);
		commandList->setRenderTargets(&framebuffers[currentBuffer], 1);

		commandList->clear(framebuffers[currentBuffer], Graphics5::ClearColorFlag);
		commandList->setPipeline(pipeline);
		commandList->setVertexBuffers(&vertices, 1);
		commandList->setIndexBuffer(*indices);
		commandList->drawIndexedVertices();

		commandList->renderTargetToFramebufferBarrier(framebuffers[currentBuffer]);
		commandList->end();
		
		Graphics5::end();
		Graphics5::swapBuffers(0);
	}
}

int kore(int argc, char** argv) {
	Kore::System::setName("Shader");
	Kore::System::setup();
	Kore::WindowOptions options;
	options.title = "Shader";
	options.width = 1024;
	options.height = 768;
	options.x = 100;
	options.y = 100;
	options.targetDisplay = -1;
	options.mode = WindowMode::WindowModeWindow;
	options.rendererOptions.depthBufferBits = 16;
	options.rendererOptions.stencilBufferBits = 8;
	options.rendererOptions.textureFormat = 0;
	options.rendererOptions.antialiasing = 0;
	Kore::System::initWindow(options);
	Kore::System::setCallback(update);

	FileReader vs("shader.vert");
	FileReader fs("shader.frag");
	vertexShader = new Graphics5::Shader(vs.readAll(), vs.size(), Graphics5::VertexShader);
	fragmentShader = new Graphics5::Shader(fs.readAll(), fs.size(), Graphics5::FragmentShader);
	Graphics4::VertexStructure structure;
	structure.add("pos", Graphics4::Float3VertexData);
	pipeline = new Graphics5::PipelineState();
	pipeline->vertexShader = vertexShader;
	pipeline->fragmentShader = fragmentShader;
	pipeline->inputLayout[0] = &structure;
	pipeline->inputLayout[1] = nullptr;
	pipeline->compile();

	commandList = new Graphics5::CommandList;
	for (int i = 0; i < bufferCount; ++i) {
		framebuffers[i] = new Graphics5::RenderTarget(System::windowWidth(0), System::windowHeight(0), 16);
	}

	vertices = new Graphics5::VertexBuffer(3, structure, false);
	float* v = vertices->lock();
	v[0] = -1; v[1] = -1; v[2] = 0.5;
	v[3] = 1;  v[4] = -1; v[5] = 0.5;
	v[6] = -1; v[7] = 1;  v[8] = 0.5;
	vertices->unlock();

	indices = new Graphics5::IndexBuffer(3, true);
	int* i = indices->lock();
	i[0] = 0; i[1] = 1; i[2] = 2;
	indices->unlock();

	Kore::System::start();

	return 0;
}
