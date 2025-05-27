#include "pch.h"
#include "CineTrack.h"
#include "TrackKeyFrame.h"

void CineTrack::InsertKeyFrame(TrackKeyFrame* frame, double time) {


	if (m_KeyFrames.size() > 0)
	{
		if (frame->GetTime() > m_KeyFrames[m_KeyFrames.size() - 1]->GetTime()) {
			m_KeyFrames.push_back(frame);
			return;
		}
	}
	if (m_KeyFrames.size() == 0) {

		m_KeyFrames.push_back(frame);
		return;
	}
	if (m_KeyFrames.size() == 1) {

		if (time < m_KeyFrames[0]->GetTime())
		{
			m_KeyFrames.insert(m_KeyFrames.begin(), frame);

		}
		else {
			m_KeyFrames.push_back(frame);
		}
		return;

	}



	int ix = 0;
	for (auto frame1 : m_KeyFrames) {

		if (frame1->GetTime() > time) {

			m_KeyFrames.insert(m_KeyFrames.begin() + ix, frame);
			return;
		}
		ix++;
	}

}

std::vector<TrackKeyFrame*> CineTrack::GetFrames(double time) {
	std::vector<TrackKeyFrame*> frames;

	if (time <= 0) {
		if (m_KeyFrames.size() == 0) return frames;
		frames.push_back(m_KeyFrames[0]);
		return frames;
	}

	if (m_KeyFrames.size() == 0) return frames;
	if (time < m_KeyFrames[0]->GetTime()) {
	
	
		frames.push_back(m_KeyFrames[0]);
		return frames;

	}

	if (time > m_KeyFrames[m_KeyFrames.size() - 1]->GetTime() || m_KeyFrames[m_KeyFrames.size()-1]->GetTime()==time)
	{
		frames.push_back(m_KeyFrames[m_KeyFrames.size() - 1]);
		return frames;
	}

	int ii = 0;
	for (auto frame : m_KeyFrames) {

		if (time > frame->GetTime()) {

		}else{
			frames.push_back(frame);
			frames.push_back(m_KeyFrames[ii-1]);
			return frames;
		}
		ii++;
	}

	int b = 5;
}