#pragma once
#include <vector>

class PostProcess;
class Draw2D;

namespace Q3D::Engine::Texture {
	class Texture2D;
}

class PostProcessing
{
public:

	PostProcessing();
	void AddPostProcess(PostProcess* process);
	void Process(Q3D::Engine::Texture::Texture2D* frame);
	void Clear() {
		m_Processes.clear();
	}

private:

	std::vector<PostProcess*> m_Processes;
	Draw2D* m_Draw;

};

