#pragma once
#include <vector>

class PostProcess;
class Draw2D;
class Texture2D;

class PostProcessing
{
public:

	PostProcessing();
	void AddPostProcess(PostProcess* process);
	void Process(Texture2D* frame);
	void Clear() {
		m_Processes.clear();
	}

private:

	std::vector<PostProcess*> m_Processes;
	Draw2D* m_Draw;

};

