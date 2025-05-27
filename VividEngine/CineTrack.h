#pragma once
#include <vector>
#include <string>

class Node;
class NodeEntity;
class NodeActor;
class TrackKeyFrame;
class Cinematic;

class CineTrack
{
public:

	void SetOwner(Cinematic* cine) {
		m_Cinematic = cine;
	}
	Cinematic* GetOwner() {
		return m_Cinematic;
	}
	void InsertKeyFrame(TrackKeyFrame* frame, double time);
	void AddKeyFrame(TrackKeyFrame* frame);
	void SetNode(Node* node)
	{
		m_Node = node;
	}
	Node* GetNode() {
		return m_Node;
	}
	std::vector<TrackKeyFrame*> GetFrames() {
		return m_KeyFrames;
	}
	std::vector<TrackKeyFrame*> GetFrames(double time);

private:

	Node* m_Node;
	NodeActor* m_Actor;
	std::vector<TrackKeyFrame*> m_KeyFrames;
	Cinematic* m_Cinematic;

};

