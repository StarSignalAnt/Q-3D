#include "CloudsSystem.h"
#include "Texture3D.h"
#include "GraphNode.h"
#include <iostream> // For logging
#include <vector>   // For creating the layer data buffer
#include <cmath>    // For sqrt
#include "MaterialClouds.h"
#include "Importer.h"
#include "GraphNode.h"
#include "StaticMeshComponent.h"
#define FNL_IMPL
#include "FastNoiseLite.h"
#include "MaterialCloudGen.h"

float map(float value, float min1, float max1, float min2, float max2) {
    return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}


CloudsSystem::CloudsSystem(CloudsQuality quality) {

	m_Quality = quality;

	switch (m_Quality) {
    case CloudsQuality::CQ_Ultra:
        m_CloudMap = new Texture3D(512, 512, 512);
        m_Width = 512;
        m_Height = 512;
        m_Depth = 512;
        break;
    case CloudsQuality::CQ_High:
        m_CloudMap = new Texture3D(256, 256, 256);
        m_Width = 256;
        m_Height = 256;
        m_Depth = 256;
        break;
	case CloudsQuality::CQ_Mid:
		m_CloudMap = new Texture3D(128, 128, 128);
		m_Width = 128;
		m_Height = 128;
		m_Depth = 128;
		break;

	}

    m_CloudMaterial = new MaterialClouds;


    auto imp = new Importer;

    m_Sky = imp->ImportEntity("Edit/Primitives/Sphere.fbx");
    m_Sky->SetScale(glm::vec3(1300, 1300, 1300));
    //m_SkyMaterial = new MaterialSky;

    for (auto sm : m_Sky->GetNodes()[0]->GetComponent<StaticMeshComponent>()->GetSubMeshes()) {

        sm->m_Material = m_CloudMaterial;

    }

    m_CloudVolumeSize = glm::vec3(3500.0f, 300.0f,3500.0f);

    // Center it on the X/Z axes and start it 2,000 units up in the sky
    m_CloudVolumeStart = glm::vec3(
        -m_CloudVolumeSize.x / 2.0f,  // -20000
        120.0f,                     // Start height
        -m_CloudVolumeSize.z / 2.0f   // -20000
    );
    m_CloudMaterial->m_Map = m_CloudMap;
    //CreateClouds(0);
    m_CloudGenMaterial = new MaterialCloudGen;
    GenerateCloudsGPU();
}

void CloudsSystem::GenerateCloudsGPU() {
    m_CloudGenMaterial->Dispatch(m_CloudMap, m_TotalTime, m_Coverage);
}


void CloudsSystem::CreateClouds(float time)
{
    if (!m_CloudMap) return;

    // 1. Setup Noise Generators
    // 1. Setup Noise Generators (Base, Erosion, and Detail)
    fnl_state baseNoiseState = fnlCreateState();
    baseNoiseState.noise_type = FNL_NOISE_PERLIN;
    baseNoiseState.frequency = 0.02f;

    // This noise adds the smaller, more turbulent details.
    fnl_state detailNoiseState = fnlCreateState();
    detailNoiseState.noise_type = FNL_NOISE_PERLIN;
    detailNoiseState.frequency = 0.1f; // Higher frequency for finer details

    for (int d = 0; d < m_Depth; ++d) {
        std::vector<uint8_t> layerData(m_Width * m_Height);

        for (int y = 0; y < m_Height; ++y) {
            for (int x = 0; x < m_Width; ++x) {

                // --- 2. Generate Cloud Coverage Mask ---
                float baseValue = fnlGetNoise3D(&baseNoiseState, (float)x, (float)y, (float)d + time);
                baseValue = map(baseValue, -1.0f, 1.0f, 0.0f, 1.0f);

                // Use smoothstep to turn the base noise into a cloud mask based on coverage.
                // This is the most important step for controlling how much sky is filled.
                float cloudCoverage = glm::smoothstep(1.0f - m_Coverage, 1.0f, baseValue);

                // --- 3. Generate Internal Cloud Detail ---
                float detailValue = fnlGetNoise3D(&detailNoiseState, (float)x, (float)y, (float)d + time);
                detailValue = map(detailValue, -1.0f, 1.0f, 0.0f, 1.0f);

                // --- 4. Combine Coverage and Detail ---
                // The final density is the cloud coverage, with the internal detail subtracted from it.
                // This carves detail out of the main cloud shape.
                float density = cloudCoverage - detailValue * 0.5f; // Reduce detail strength slightly
                density = std::max(0.0f, density); // Ensure density doesn't go below zero

                // --- 5. Apply Vertical Gradient & Edge Falloff ---
                float normY = (float)y / m_Height;
                float verticalGradient = glm::smoothstep(0.2f, 0.45f, normY);
                density *= verticalGradient;

                float normX = (float)x / m_Width;
                float normZ = (float)d / m_Depth;
                float distX = abs(normX - 0.5f) * 2.0f;
                float distY = abs(normY - 0.5f) * 2.0f;
                float distZ = abs(normZ - 0.5f) * 2.0f;
                float distFromCenter = sqrt(pow(distX, 2) + pow(distY, 2) + pow(distZ, 2));
                float edgeFade = 1.0 - glm::smoothstep(0.8f, 1.0f, distFromCenter);
                density *= edgeFade;

                layerData[(y * m_Width) + x] = (uint8_t)(density * 255.0f);
            }
        }
        m_CloudMap->UploadLayer(layerData.data(), d);
    }
}


void CloudsSystem::CreateTestCloudMap()
{
    return;
    if (!m_CloudMap) {
        std::cerr << "Cannot create test map, m_CloudMap is null." << std::endl;
        return;
    }

    std::cout << "Generating test cloud map..." << std::endl;

    // Define the sphere's properties
    float centerX = m_Width / 2.0f;
    float centerY = m_Height / 2.0f;
    float centerZ = m_Depth / 2.0f;
    float radius = m_Width / 4.0f; // A reasonable radius
    float radiusSq = radius * radius; // Use squared distance for efficiency

    // Iterate through each 2D layer of the 3D texture
    for (int d = 0; d < m_Depth; ++d) {
        // Create a buffer for the current layer's data.
        // The texture format is R8_UNORM, so we use uint8_t (0-255).
        std::vector<uint8_t> layerData(m_Width * m_Height);

        // Populate the layer data
        for (int y = 0; y < m_Height; ++y) {
            for (int x = 0; x < m_Width; ++x) {
                // Calculate the squared distance from the center of the sphere
                float dx = x - centerX;
                float dy = y - centerY;
                float dz = d - centerZ;
                float distSq = (dx * dx) + (dy * dy) + (dz * dz);

                // If the point is inside the sphere, set density to max (255), otherwise min (0)
                if (distSq <= radiusSq) {
                    layerData[(y * m_Width) + x] = 255;
                }
                else {
                    layerData[(y * m_Width) + x] = 0;
                }
            }
        }

        // Upload the completed 2D layer to the correct depth slice of the 3D texture
        m_CloudMap->UploadLayer(layerData.data(), d);
    }

 

    std::cout << "Test cloud map generation complete." << std::endl;
}


void CloudsSystem::Update(float dt) {

   

    m_CloudMaterial->m_SunDir = m_SunDir;
    m_CloudMaterial->m_VolStart = m_CloudVolumeStart;
    m_CloudMaterial->m_VolSize = m_CloudVolumeSize;
    m_TotalTime += dt * m_AnimationSpeed * 0.1;
    GenerateCloudsGPU();

}

void CloudsSystem::Render(GraphNode* camera) {

    m_Sky->Render(camera);

}

void CloudsSystem::SetMesh(GraphNode* sky) {

    m_Sky = sky;

}
void CloudsSystem::SetSunDir(glm::vec3 dir) {

    m_SunDir = dir;

}