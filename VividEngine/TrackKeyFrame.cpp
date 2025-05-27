#include "pch.h"
#include "TrackKeyFrame.h"

void TrackKeyFrame::SetPosition(float3 position) {

	m_Position = position;

}

void TrackKeyFrame::SetRotation(float4x4 rotation) {

	m_Rotation = rotation;

}

void TrackKeyFrame::SetScale(float3 scale) {

	m_Scale = scale;

}

float3 TrackKeyFrame::GetPosition() {

	return m_Position;

}

float4x4 TrackKeyFrame::GetRotation() {

	return m_Rotation;

}

float3 TrackKeyFrame::GetScale() {

	return m_Scale;

}

void TrackKeyFrame::SetTime(double time) {

	m_Time = time;

}

double TrackKeyFrame::GetTime() {

	return m_Time;

}