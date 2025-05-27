#pragma once
#include <vector>
#include <string>

class CineTrack;

class Cinematic
{
public:

	void AddTrack(CineTrack* track) {
		m_Tracks.push_back(track);
	}
	void SetLength(double length) {
		m_Length = length;
	}
	double GetLength() {
		return m_Length;
	}
	void SetTime(double time);
private:
	
	double m_Length = 0;
	std::vector<CineTrack*> m_Tracks;

};


