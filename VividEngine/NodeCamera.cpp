#include "pch.h"
#include "NodeCamera.h"
#include "Engine.h"
#include "VFile.h"

int cam_index = 1;
NodeCamera::NodeCamera() {

    m_Name = "Camera" + std::to_string(cam_index);
    cam_index++;

}

float4x4 NodeCamera::GetProjection() {


	float FOVRadians = m_FOV * (3.14159265358979323846f / 180.0f);
	return float4x4::Projection(FOVRadians, (float)Engine::GetFrameWidth() / (float)Engine::GetFrameHeight(), m_NearZ, m_FarZ, true);

}

float4x4 NodeCamera::GetProjection2D() {

	return float4x4::OrthoOffCenter(0, Engine::GetFrameWidth(), Engine::GetFrameHeight(), 0, 0.01f, 10.0f,false);// Projection(FOVRadians, (float)Engine::GetFrameWidth() / (float)Engine::GetFrameHeight(), m_NearZ, m_FarZ, true);


}

float4x4 NodeCamera::GetWorldMatrix() {

	return Node::GetWorldMatrix().Inverse();

}

int NodeCamera::InView(float3 centre, float3 size) {
    float4x4 viewProjMatrix = GetWorldMatrix() * GetProjection();
    float4x4 vpMatrix = viewProjMatrix;
    // Frustum planes from view-projection matrix
    float4 planes[6];

    // Left
    planes[0] = { vpMatrix._14 + vpMatrix._11, vpMatrix._24 + vpMatrix._21, vpMatrix._34 + vpMatrix._31, vpMatrix._44 + vpMatrix._41 };
    // Right
    planes[1] = { vpMatrix._14 - vpMatrix._11, vpMatrix._24 - vpMatrix._21, vpMatrix._34 - vpMatrix._31, vpMatrix._44 - vpMatrix._41 };
    // Bottom
    planes[2] = { vpMatrix._14 + vpMatrix._12, vpMatrix._24 + vpMatrix._22, vpMatrix._34 + vpMatrix._32, vpMatrix._44 + vpMatrix._42 };
    // Top
    planes[3] = { vpMatrix._14 - vpMatrix._12, vpMatrix._24 - vpMatrix._22, vpMatrix._34 - vpMatrix._32, vpMatrix._44 - vpMatrix._42 };
    // Near
    planes[4] = { vpMatrix._13, vpMatrix._23, vpMatrix._33, vpMatrix._43 };
    // Far
    planes[5] = { vpMatrix._14 - vpMatrix._13, vpMatrix._24 - vpMatrix._23, vpMatrix._34 - vpMatrix._33, vpMatrix._44 - vpMatrix._43 };

    // Normalize the planes
    for (int i = 0; i < 6; ++i) {
        float length = sqrtf(planes[i].x * planes[i].x + planes[i].y * planes[i].y + planes[i].z * planes[i].z);
        planes[i] /= length;
    }

    // Check each plane
    float3 halfSize = size / 2.0f;
    float3 corners[8] = {
        { centre.x - halfSize.x, centre.y - halfSize.y, centre.z - halfSize.z },
        { centre.x + halfSize.x, centre.y - halfSize.y, centre.z - halfSize.z },
        { centre.x - halfSize.x, centre.y + halfSize.y, centre.z - halfSize.z },
        { centre.x + halfSize.x, centre.y + halfSize.y, centre.z - halfSize.z },
        { centre.x - halfSize.x, centre.y - halfSize.y, centre.z + halfSize.z },
        { centre.x + halfSize.x, centre.y - halfSize.y, centre.z + halfSize.z },
        { centre.x - halfSize.x, centre.y + halfSize.y, centre.z + halfSize.z },
        { centre.x + halfSize.x, centre.y + halfSize.y, centre.z + halfSize.z }
    };

    bool fullyInside = true;
    for (int i = 0; i < 6; ++i) {
        int insideCount = 0;
        for (const auto& corner : corners) {
            float distance = planes[i].x * corner.x + planes[i].y * corner.y + planes[i].z * corner.z + planes[i].w;
            if (distance >= 0) {
                insideCount++;
            }
        }
        if (insideCount == 0) {
            // All corners are outside this plane
            return 0; // Fully hidden
        }
        else if (insideCount != 8) {
            fullyInside = false;
        }
    }

    return fullyInside ? 2 : 1;
}

void NodeCamera::WriteNode(VFile* file) {

    file->WriteInt(5);

    file->WriteVec3(m_Position);
    file->WriteMatrix(m_Rotation);
    file->WriteVec3(m_Scale);
    file->WriteString(m_Name.c_str());
    file->WriteFloat(m_NearZ);
    file->WriteFloat(m_FarZ);
    file->WriteFloat(m_FOV);

    WriteScripts(file);

    file->WriteInt(m_Nodes.size());

    for (auto node : m_Nodes) {

        node->WriteNode(file);

    }


}

void NodeCamera::ReadNode(VFile* file) {

    m_Position = file->ReadVec3();
    m_Rotation = file->ReadMatrix();
    m_Scale = file->ReadVec3();
    m_Name = file->ReadString();
    m_NearZ = file->ReadFloat();
    m_FarZ = file->ReadFloat();
    m_FOV = file->ReadFloat();

    



}