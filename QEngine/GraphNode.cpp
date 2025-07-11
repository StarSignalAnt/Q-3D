
#include "GraphNode.h"
#include "Component.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include "BasicMath.hpp"
#include <glm/glm.hpp>
#include "StaticMeshComponent.h"
#include "Physics.h"
#include "QEngine.h"
#include "Importer.h"
#include "ScriptComponent.h"
#include "SharpComponent.h"
#include "SceneGraph.h"
#include "LightComponent.h" 
#include "Properties.h"
#include <glm/gtc/quaternion.hpp>
using namespace Diligent;
physx::PxQuat glmMat4ToPxQuat(const glm::mat4& rotationMatrix)
{
	glm::quat q = glm::quat_cast(rotationMatrix);  // Convert mat4 to glm::quat
	return physx::PxQuat(q.x, q.y, q.z, q.w);      // Convert to PxQuat
}


const std::vector<std::pair<BodyType, std::string>> BodyTypeOptions = {
{ BodyType::T_None, "None" },
{ BodyType::T_Box, "Box" },
{ BodyType::T_ConvexHull,"Convex Hull"},
{ BodyType::T_TriMesh,"Tri Mesh"},
{ BodyType::T_Sphere,"Sphere" }
};

GraphNode::GraphNode() {

	Reset();
	m_Components = std::vector<Component*>();
	m_Name = "Node";

	m_Properties.bind("Name", &m_Name);
	m_Properties.bind("Position", &m_Position);
	glm::vec3 tmp = glm::vec3(0, 0, 0);
	m_Properties.bind("Rotation", &tmp);
	m_Properties.bind("Scale", &m_Scale);
	m_Properties.bind_enum("Body Type", &m_BodyType, BodyTypeOptions);

}

void GraphNode::Reset() {
	
	m_Position = glm::vec3(0, 0, 0);
	m_Scale = glm::vec3(1, 1, 1);
	m_Rotation = glm::mat4(1.0f);
	MarkBoundsAsDirty();
	m_Updated = true;

}

void GraphNode::AddNode(GraphNode* node) {
	if (node == nullptr) return;
	m_Nodes.push_back(node);
	node->SetRootNode(this);
}


void GraphNode::AddComponent(Component* component) {
	component->OnAttach(this);
	m_Components.push_back(component);
}



void GraphNode::Render(GraphNode* camera) {

	for(auto & component : m_Components) {
		component->OnRender(camera);
	}
//	for (auto& component : m_CComponents) {
	//	component->OnRender(camera);
//	}


	for (auto sub : m_Nodes) {
		sub->Render(camera);
	}

}

void GraphNode::RenderDepth(GraphNode* camera) {

	for (auto& component : m_Components) {
		component->OnRenderDepth(camera);
	}
//	for (auto& component : m_CComponents) {
//		component->OnRenderDepth(camera);
	//}

	for (auto sub : m_Nodes) {
		sub->RenderDepth(camera);
	}

}

glm::mat4 GraphNode::GetWorldMatrix(){

	glm::mat4 root = glm::mat4(1.0f);

	if (m_RootNode != nullptr) {
		root = m_RootNode->GetWorldMatrix();
	}

	glm::mat4 transform = glm::mat4(1.0f);
	auto pos = glm::translate(transform, m_Position);
	auto rot  = m_Rotation;
	auto scale  = glm::scale(transform, m_Scale);
	return root* (pos * rot * scale);

}

void GraphNode::SetPosition(glm::vec3 position) {

	m_Position = position;
	MarkBoundsAsDirty();
}

void GraphNode::SetRotation(glm::vec3 rotation) {
    // Reset the rotation matrix
    m_Rotation = glm::mat4(1.0f);

    // Convert to radians and apply rotations in ZYX order
    glm::vec3 radians = glm::radians(rotation);

    m_Rotation = glm::rotate(m_Rotation, radians.z, glm::vec3(0, 0, 1)); // Roll
    m_Rotation = glm::rotate(m_Rotation, radians.y, glm::vec3(0, 1, 0)); // Yaw
    m_Rotation = glm::rotate(m_Rotation, radians.x, glm::vec3(1, 0, 0)); // Pitch
	MarkBoundsAsDirty();
}

void GraphNode::SetScale(glm::vec3 scale) {
	m_Scale = scale;
	MarkBoundsAsDirty();
}

void GraphNode::Move(glm::vec3 offset) {
	glm::vec3 rotatedOffset = glm::vec3(m_Rotation * glm::vec4(offset, 1.0f));
	m_Position += rotatedOffset;
	m_Updated = true;
	
	if (GetComponent<StaticMeshComponent>()!=nullptr) {

		printf("Updating!!\n");
		GetComponent<StaticMeshComponent>()->Updated();

	}
	MarkBoundsAsDirty();
}

glm::mat4 f42g(const Diligent::float4x4& diligentMat)
{
	
	return glm::mat4(
		diligentMat._11, diligentMat._21, diligentMat._31, diligentMat._41,  // GLM column 0
		diligentMat._12, diligentMat._22, diligentMat._32, diligentMat._42,  // GLM column 1
		diligentMat._13, diligentMat._23, diligentMat._33, diligentMat._43,  // GLM column 2
		diligentMat._14, diligentMat._24, diligentMat._34, diligentMat._44   // GLM column 3
	);

}

void GraphNode::SetRotation(float4x4 rot) {

	m_Rotation = f42g(rot);
	MarkBoundsAsDirty();
}

void GraphNode::Update(float dt) {

	 
	for (auto& component : m_Components) {
		component->OnUpdate(dt);
	}

	//for (auto& component : m_CComponents) {
	//	component->OnUpdate(dt);
	//}

	for (auto sub : m_Nodes) {
		sub->Update(dt);
	}

}

void GraphNode::Turn(glm::vec3 delta, bool local) {

	auto rot = glm::mat4(1.0f);

	// Convert to radians and apply rotations in ZYX order
	glm::vec3 radians = glm::radians(delta);


	rot = glm::rotate(rot, radians.z, glm::vec3(0, 0, 1)); // Roll
	rot = glm::rotate(rot, radians.y, glm::vec3(0, 1, 0)); // Yaw
	rot = glm::rotate(rot, radians.x, glm::vec3(1, 0, 0)); // Pitch

	if (local) {
		m_Rotation = m_Rotation * rot;
	}
	else {
		m_Rotation = rot * m_Rotation;
	}
	MarkBoundsAsDirty();
}

void GraphNode::Translate(glm::vec3 delta) {

	m_Position += delta;
	MarkBoundsAsDirty();
}

void GraphNode::Push() {

	m_PositionPush = m_Position;
	m_ScalePush = m_Scale;
	m_RotationPush = m_Rotation;

	for (auto comp : m_Components) {

		comp->Push();

	}

	for (auto node : m_Nodes) {
		node->Push();
	}
	
	MarkBoundsAsDirty();
}

void GraphNode::Pop() {

	m_Position = m_PositionPush;
	m_Scale = m_ScalePush;
	m_Rotation = m_RotationPush;

	for (auto comp : m_Components) {

		comp->Pop();

	}

	for (auto node : m_Nodes) {
		node->Pop();
	}
	MarkBoundsAsDirty();
}

glm::vec3 GraphNode::GetWorldPosition() {

	glm::vec3 root = glm::vec3(0, 0, 0);

	if (m_RootNode) {

		root = m_RootNode->GetWorldPosition();

	}

	return root + m_Position;

}

glm::mat4 GraphNode::GetWorldRotation() {

	glm::mat4 root = glm::mat4(1.0f);

	if (m_RootNode) {
		root = m_RootNode->GetWorldRotation();
		
	}

	return root * m_Rotation;

}

void GraphNode::Play() {

	for (auto comp : m_Components) {

		comp->OnPlay();

	}

	for (auto node : m_Nodes) {

		node->Play();

	}

	switch (m_BodyType) {
	case T_Box:
		if (m_RB != nullptr) {
			QEngine::GetPhysics()->RemoveActor(m_RB);
		}
		//SetBody(BodyType::T_Box);
		CreateRB();
		CreateBody();

		break;
	case T_TriMesh:

		if (m_RS != nullptr) {
			QEngine::GetPhysics()->RemoveStatic(m_RS);
		}


		CreateBody();

		break;
	case T_ConvexHull:
		if (m_RB != nullptr) {
			QEngine::GetPhysics()->RemoveActor(m_RB);
		}
		CreateBody();
		break;
	}

}

void GraphNode::Stop() {

	for (auto comp : m_Components) {

		comp->OnStop();

	}

	for (auto node : m_Nodes) {

		node->Stop();

	}

}

Bounds GraphNode::GetStaticMeshBounds(bool includeChildren) {
	Bounds totalBounds;
	bool foundAnyVertices = false;

	if (m_bBoundsAreDirty) {
		m_Bounds = nullptr;
	}

	if (m_Bounds == nullptr) {
		// Get all StaticMeshComponent instances from this node
		auto staticMeshComponents = GetComponents<StaticMeshComponent>();
		QEngine::DebugLog("Getting bounds");
		m_bBoundsAreDirty = false;


		for (auto meshComp : staticMeshComponents) {
			const auto& subMeshes = meshComp->GetSubMeshes();

			for (const auto& subMesh : subMeshes) {
				if (subMesh->m_LODs[0]->m_Vertices.empty()) continue;

				// Get world matrix for this submesh's owner.
				// This matrix includes the position, rotation, and scale of the GraphNode.
				glm::mat4 worldMatrix = this->GetWorldMatrix();

				// Process each vertex
				for (const auto& vertex : subMesh->m_LODs[0]->m_Vertices) {
					// *** FIX: Transform vertex from local space to world space ***
					glm::vec3 worldPos3 = glm::vec3(worldMatrix * glm::vec4(vertex.position, 1.0f));

					if (!foundAnyVertices) {
						// First vertex - initialize bounds
						totalBounds.min = worldPos3;
						totalBounds.max = worldPos3;
						foundAnyVertices = true;
					}
					else {
						// Expand bounds using the world-space position
						totalBounds.min = glm::min(totalBounds.min, worldPos3);
						totalBounds.max = glm::max(totalBounds.max, worldPos3);
					}
				}
			}
		}

		// Recursively include children if requested
		if (includeChildren) {
			for (const auto& childNode : m_Nodes) {
				Bounds childBounds = childNode->GetStaticMeshBounds(true);

				if (childBounds.IsValid()) {
					if (!foundAnyVertices) {
						totalBounds = childBounds;
						foundAnyVertices = true;
					}
					else {
						totalBounds.min = glm::min(totalBounds.min, childBounds.min);
						totalBounds.max = glm::max(totalBounds.max, childBounds.max);
					}
				}
			}
		}

		// Calculate derived values (size and center)
		totalBounds.CalculateDerivedValues();
		m_Bounds = new Bounds;
		m_Bounds->min = totalBounds.min;
		m_Bounds->max = totalBounds.max;
		m_Bounds->size = totalBounds.size;
		m_Bounds->center = totalBounds.center;
	}

	return *m_Bounds;
}
void GraphNode::SetBody(BodyType type) {

	m_BodyType = type;

}

glm::mat4 PxQuatToGlmMat4(const physx::PxQuat& quat) {
	// Convert PxQuat to glm::quat (note: glm::quat constructor order is w, x, y, z)
	glm::quat glmQuat(quat.w, quat.x, quat.y, quat.z);

	// Normalize just in case
	glmQuat = glm::normalize(glmQuat);

	// Convert glm quaternion to 4x4 rotation matrix
	glm::mat4 rotationMatrix = glm::mat4_cast(glmQuat);

	return rotationMatrix;
}

void GraphNode::CreateBody() {
	PxMaterial* material = QEngine::GetPhysics()->CreateMaterial(); // static friction, dynamic friction, restitution


	switch (m_BodyType)
	{
	case BodyType::T_Box:
	{
		auto bounds = GetStaticMeshBounds(true);

		auto size = bounds.max - bounds.min;

		m_BoxBody = PxBoxGeometry(size.x/2.0, size.y/2.0, size.z/2.0);
		m_Shape = PxRigidActorExt::createExclusiveShape(*m_RB, m_BoxBody, *material);

		QEngine::GetPhysics()->AddActor(m_RB);
	}
		break;
	case BodyType::T_TriMesh:
	{
		std::vector<PxVec3> vertices;   // Your mesh's vertex positions
		std::vector<PxU32> indices;
		PxU32 base = 0;
		for (auto comp : GetComponents<StaticMeshComponent>())
		{



			for (auto mesh : comp->GetSubMeshes()) {

				for (auto v : mesh->m_LODs[0]->m_Vertices) {

					auto p = GetWorldMatrix() * glm::vec4(v.position, 1.0);

					PxVec3 pos = PxVec3(p.x,p.y,p.z);
					vertices.push_back(pos);

				}


				for (auto t : mesh->m_LODs[0]->m_Triangles) {

					indices.push_back(base + (PxU32)t.v0);
					indices.push_back(base + (PxU32)t.v1);
					indices.push_back(base + (PxU32)t.v2);
					indices.push_back(base + (PxU32)t.v0);
					indices.push_back(base + (PxU32)t.v2);
					indices.push_back(base + (PxU32)t.v1);


				}

				base += mesh->m_LODs[0]->m_Vertices.size();



			}

		}
		PxTriangleMeshDesc meshDesc;
		meshDesc.points.count = static_cast<PxU32>(vertices.size());
		meshDesc.points.stride = sizeof(PxVec3);
		meshDesc.points.data = vertices.data();

		meshDesc.triangles.count = static_cast<PxU32>(indices.size()) / 3;
		meshDesc.triangles.stride = 3 * sizeof(PxU32);
		meshDesc.triangles.data = indices.data();

		PxDefaultMemoryOutputStream writeBuffer;
		PxTriangleMeshCookingResult::Enum result;
		bool success = QEngine::GetPhysics()->GetCooking()->cookTriangleMesh(meshDesc, writeBuffer, &result);
		if (!success || result != PxTriangleMeshCookingResult::eSUCCESS)
		{
			throw std::runtime_error("Failed to cook triangle mesh!");
		}

		// 3. Create triangle mesh from cooked data
		PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
		PxTriangleMesh* triMesh = QEngine::GetPhysics()->GetPhysics()->createTriangleMesh(readBuffer);

		// 4. Create triangle mesh geometry
		PxTriangleMeshGeometry triGeom(triMesh);

		// 5. Create a static actor
		PxTransform pose(PxVec3(0.0f, 0.0f, 0.0f));  // Position in world
		PxRigidStatic* staticActor = QEngine::GetPhysics()->GetPhysics()->createRigidStatic(pose);

		// 6. Attach shape to static actor
		physx::PxShape* shape = PxRigidActorExt::createExclusiveShape(*staticActor, triGeom, *material);
		// 7. Add actor to scene

		QEngine::GetPhysics()->AddStatic(staticActor);
		m_RS = staticActor;
	}
	

		break;
	case T_ConvexHull:
		std::vector<PxVec3> vertices;
		for (auto comp : GetComponents<StaticMeshComponent>())
		{
			for (auto mesh : comp->GetSubMeshes())
			{
				for (auto& v : mesh->m_LODs[0]->m_Vertices)
				{
					glm::vec4 worldPos = glm::vec4(v.position, 1.0f);
					vertices.emplace_back(worldPos.x, worldPos.y, worldPos.z);
				}
			}
		}

		// 1. Convex mesh descriptor
		PxConvexMeshDesc convexDesc;
		convexDesc.points.count = static_cast<PxU32>(vertices.size());
		convexDesc.points.stride = sizeof(PxVec3);
		convexDesc.points.data = vertices.data();
		convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

		// 2. Cook convex mesh
		PxDefaultMemoryOutputStream writeBuffer;
		PxConvexMeshCookingResult::Enum result;
		bool success = QEngine::GetPhysics()->GetCooking()->cookConvexMesh(convexDesc, writeBuffer, &result);
		if (!success || result != PxConvexMeshCookingResult::eSUCCESS)
		{
			throw std::runtime_error("Failed to cook convex hull!");
		}

		PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
		PxConvexMesh* convexMesh = QEngine::GetPhysics()->GetPhysics()->createConvexMesh(readBuffer);

		// 3. Create convex geometry
		PxConvexMeshGeometry convexGeom(convexMesh);

		// 4. Create dynamic rigid body
		PxTransform transform(PxVec3(0.0f, 0.0f, 0.0f));  // Or set based on your object
		PxRigidDynamic* dynamicActor = QEngine::GetPhysics()->GetPhysics()->createRigidDynamic(transform);

		// 5. Create shape and attach to actor
		m_Shape = PxRigidActorExt::createExclusiveShape(*dynamicActor, convexGeom, *material);
		PxTransform t;
		t.p = PxVec3(m_Position.x, m_Position.y, m_Position.z);
		t.q = glmMat4ToPxQuat(m_Rotation);
		dynamicActor->setGlobalPose(t, true);

		// 6. Add actor to scene
		QEngine::GetPhysics()->AddActor(dynamicActor);
		m_RB = dynamicActor;  // Store handle to the rigid body
		break;
	}



}

void GraphNode::CreateRB() {

	m_RB = QEngine::GetPhysics()->CreateRB();
	
	PxTransform t;
	t.p = PxVec3(m_Position.x, m_Position.y, m_Position.z);
	t.q = glmMat4ToPxQuat(m_Rotation);
	m_RB->setGlobalPose(t, true);



}

void GraphNode::UpdatePhysics() {

	if (m_RB) {
		PxTransform t = m_RB->getGlobalPose();
		m_Position = glm::vec3(t.p.x, t.p.y, t.p.z);
		m_Rotation = PxQuatToGlmMat4(t.q);
	}

	for (auto node : m_Nodes) {
		node->UpdatePhysics();
	}

}

void GraphNode::Write(VFile* f) {

	f->WriteVec3(m_Position);
	f->WriteVec3(m_Scale);
	f->WriteMatrix(m_Rotation);
	f->WriteString(m_Name.c_str());
	f->WriteBool(m_HideFromEditor);

	


	f->WriteInt(m_Nodes.size());

	for (int i = 0; i < m_Nodes.size(); i++) {

		m_Nodes[i]->Write(f);

	}


}

void GraphNode::Read(VFile* f)
{
	m_Position = f->ReadVec3();
	m_Scale = f->ReadVec3();
	m_Rotation = f->ReadMatrix();
	m_Name = f->ReadString();
	m_HideFromEditor = f->ReadBool();


	int nc = f->ReadInt();

	for (int i = 0; i < nc; i++)
	{

		auto sub = new GraphNode;
		m_Nodes.push_back(sub);
		sub->Read(f);
	}


	//AddNode(sub);
}



void GraphNode::ReadScripts(VFile* f) {


	int cs = f->ReadInt();

	for (int i = 0; i < cs; i++) {



		std::string name = f->ReadString();

		auto comps = QEngine::GetCComponents();
		Component* inst = nullptr;
		for (auto c : comps) {

			if (c->GetName() == name) {

				inst = c->CreateInstance();
				AddComponent(inst);
				break;

			}

		}

		auto& props = inst->GetProperties();
		int pc = f->ReadInt();

		for (auto prop : props) {
			PropertyType pt = (PropertyType)f->ReadInt();



			switch (pt) {
			case PropertyType::STRING:

				props.set(prop, (std::string)f->ReadString());

				break;
			case PropertyType::FLOAT:

				props.set(prop, f->ReadFloat());

				break;
			case PropertyType::INT:

				props.set(prop, f->ReadInt());

				break;
			case PropertyType::VEC3:

				props.set(prop, f->ReadVec3());

				break;
			}

		}

		inst->Initialize();

	}


	int shc = f->ReadInt();

	for (int i = 0; i < shc; i++) {

		std::string name = f->ReadString();
		SharpComponent* comp = new SharpComponent;


		bool proxy = false;
		auto cls = QEngine::GetMonoLib()->GetClass(name);
		if (cls == nullptr) {
			proxy = true;


		}

		MClass* cls1 = nullptr;
		if (!proxy) {
			AddComponent(comp);
			comp->SetClass(cls, QEngine::GetMonoLib()->GetAssembly(), QEngine::GetMonoLib()->GetVivid());
			comp->SetName(name);
			// Refresh the properties editor - use the safe pointer


			cls1 = comp->GetClass();
		}
		else {
			
		}
		int fc = f->ReadInt();

		for (int j = 0; j < fc; j++) {

			auto name = f->ReadString();
			SharpType type = (SharpType)f->ReadInt();

			switch (type) {
			case SHARP_TYPE_INT:
			{
				int iv = f->ReadInt();

				if (!proxy) {
					cls1->SetFieldValue<int>(name, iv);
				}
			}
				break;
			case SHARP_TYPE_FLOAT:
			{
				float fv = f->ReadFloat();
				if (!proxy) {
				
					cls1->SetFieldValue<float>(name, fv);
				}
			}
				break;
			case SHARP_TYPE_STRING:
			{
				std::string sv = f->ReadString();
				if (!proxy) {
				
					cls1->SetFieldValue<std::string>(name, sv);
				}
			}
				break;
			case SHARP_TYPE_VEC3:
			{
				glm::vec3 v3v = f->ReadVec3();

				//
				float* nv = new float[3];
				nv[0] = v3v.x;
				nv[1] = v3v.y;
				nv[2] = v3v.z;

				if (!proxy) {
					cls1->SetFieldStruct(name, nv);
				}
			}
				break;
			case SHARP_TYPE_VOID:

				if (f->ReadBool()) {

					auto name = f->ReadString();

				}

				break;
			}

		}


		


	}

	int sc = f->ReadInt();

	for (int i = 0; i < sc; i++) {

		auto path = f->ReadString();
		auto name = f->ReadString();
		auto com = new ScriptComponent;
		AddComponent(com);
		com->SetScript(path, name);

		int vc = f->ReadInt();
		for (int v = 0; v < vc; v++) {

		
			int t = (SVarType)f->ReadInt();
			std::string name = f->ReadString();
			bool gn = false;
			if (f->ReadBool()) {
				gn = true;
			}
			switch (t) {
			case VT_Object:
				if (gn) {
					if (f->ReadBool()) {
						std::string namel = f->ReadString();
						auto node = SceneGraph::m_Instance->FindNode(namel);
						com->SetNode(name, node);

						int b = 5;
					}
				}
				break;
			case VT_Int:
				com->SetInt(name, f->ReadInt());
				break;
			case VT_String:
				com->SetString(name, f->ReadString());
				break;
			case VT_Float:
				com->SetFloat(name, f->ReadFloat());
				break;
			}

		}



	}

	for (int i = 0; i < m_Nodes.size(); i++) {
		m_Nodes[i]->ReadScripts(f);
	}

}

void GraphNode::WriteScripts(VFile* f) {

	f->WriteInt(m_Components.size());

	for (int i = 0; i < m_Components.size(); i++) {

		auto comp = m_Components[i];




		//if (QEngine::IsCComponent(comp))
		//{
		//	f->WriteBool(true);
		f->WriteString(comp->GetName().c_str());
		if (comp->GetName() == "Component")
		{
			int b = 5;
		}
		//	}
		//	else {
		//		f->WriteBool(false);
	//		}
		auto props = m_Components[i]->GetProperties();

		f->WriteInt(props.size());
		for (auto prop : props) {

			auto ty = props.GetType(prop);

			f->WriteInt((int)ty);

			switch (ty) {
			case PropertyType::STRING:
			{
				std::string path = *props.get<std::string>(prop);

				f->WriteString(path.c_str());
			}
				break;
			case PropertyType::FLOAT:

				f->WriteFloat(*props.get<float>(prop));

				break;
			case PropertyType::INT:

				f->WriteInt(*props.get<int>(prop));

				break;

			case PropertyType::VEC3:

				f->WriteVec3(*props.get<glm::vec3>(prop));

				break;
			}

			//f->WriteString(prop.)

		}

	}

	for (int i = 0; i < m_Components.size(); i++) {

		
	}


	f->WriteInt(GetComponents<SharpComponent>().size());

	for (auto sc : GetComponents<SharpComponent>()) {

		f->WriteString(sc->GetName().c_str());

		auto vars = sc->GetClass()->GetInstanceFields();
		f->WriteInt(vars.size());

		for (auto v : sc->GetClass()->GetInstanceFields())
		{

			f->WriteString(v.name.c_str());
			f->WriteInt((int)v.ctype);

			switch (v.ctype) {
			case SHARP_TYPE_VOID:
			{
				{
					auto cls = v.etype;


					if (cls == "Vivid.Scene.GraphNode")
					{
						auto node = sc->GetClass()->GetFieldValue<MClass*>(v.name);

						std::string name = "None";
						if (node != nullptr) {
							//name = no
							auto r = node->CallFunctionValue_String("GetName");
							name = r;
							int b = 5;
						}
						else {

							name = "None";



						}
						f->WriteBool(true);
						f->WriteString(name.c_str());

					}
					else {
						f->WriteBool(false);
					}
				}
				break;
			case SHARP_TYPE_INT:
			{
				auto ival = sc->GetClass()->GetFieldValue<int>(v.name);

				f->WriteInt(ival);

			}
			break;
			case SHARP_TYPE_FLOAT:
			{
				auto fval = sc->GetClass()->GetFieldValue<float>(v.name);
				f->WriteFloat(fval);
			}
			break;
			case SHARP_TYPE_STRING:
			{
				std::string sval = sc->GetClass()->GetFieldValue<std::string>(v.name).c_str();
				f->WriteString(sval.c_str());

			}

			break;
			case SHARP_TYPE_VEC3:
			{
				auto val = sc->GetClass()->GetFieldValue<MClass*>(v.name);

				auto vx = val->GetFieldValue<float>("x");
				auto vy = val->GetFieldValue<float>("y");
				auto vz = val->GetFieldValue<float>("z");

				f->WriteVec3(glm::vec3(vx, vy, vz));

			}
			}

			}

		}

	}

	

	f->WriteInt(GetComponents<ScriptComponent>().size());

	for (auto s : GetComponents<ScriptComponent>()) {

		f->WriteString(s->GetPath().c_str());
		f->WriteString(s->GetName().c_str());

		auto vars = s->GetPythonVars();
		f->WriteInt(s->GetPythonVars().size());
		for (auto v : s->GetPythonVars()) {
			f->WriteInt((int)v.type);
			f->WriteString(v.name.c_str());
			if (v.cls == "GraphNode")
			{
				f->WriteBool(true);
			}
			else {
				f->WriteBool(false);
			}
			switch (v.type)
			{
			case VT_Object:
			{
				if (v.cls == "GraphNode") {
					auto val = s->GetNode(v.name);
					if (val == nullptr) {
						f->WriteBool(false);
					}
					else {
						f->WriteBool(true);
						f->WriteString(val->GetLongName().c_str());
						int b = 5;
					}
				}
			}
				break;
			case VT_Int:
				f->WriteInt(s->GetInt(v.name));
				break;
			case VT_Float:
				f->WriteFloat(s->GetFloat(v.name));
				break;
			case VT_String:
				f->WriteString(s->GetString(v.name).c_str());
				break;
			}

		}

	}

	//f->WriteInt(m_Nodes.size());
	for (auto n : m_Nodes) {
		n->WriteScripts(f);
	}

}

GraphNode* GraphNode::FindNode(std::string name) {


	if (GetLongName() == name) {
		return this;
	}

	for (auto n : m_Nodes) {
		
		auto r = n->FindNode(name);
		if (r != nullptr) return r;

	}
	return nullptr;

}

void GraphNode::RemoveParent() {

	m_RootNode = nullptr;

}

void GraphNode::MarkBoundsAsDirty() {
	m_bBoundsAreDirty = true;
	for (auto child : m_Nodes) {
		child->MarkBoundsAsDirty();
	}
}
void to_json(json& j, const glm::vec3& v) {
	j = { v.x, v.y, v.z };
}
void from_json(const json& j, glm::vec3& v) {
	j.at(0).get_to(v.x);
	j.at(1).get_to(v.y);
	j.at(2).get_to(v.z);
}

namespace nlohmann {
	

	
}
// --- END HELPER FUNCTIONS ---
void GraphNode::JWrite(json& j)  {
	// Write basic properties
	j["name"] = m_Name;
	j["position"] = m_Position;
	j["scale"] = m_Scale;
	j["rotation"] = m_Rotation;
	j["hide_from_editor"] = m_HideFromEditor;

	// --- REVISED LOGIC ---
	// Actively check for a light component. This is the definitive way to identify a light node.
	auto* light_comp = const_cast<GraphNode*>(this)->GetComponent<LightComponent>();
	if (light_comp) {
		j["resource_type"] = ResourceType::Light; // Mark this node as a light for loading
		json light_props;
		light_props["color"] = light_comp->GetColor();
		light_props["range"] = light_comp->GetRange();
		light_props["intensity"] = light_comp->GetIntensity();
		j["light_properties"] = light_props;
	}
	// Handle static meshes if no light is present
	else if (m_ResourceType == ResourceType::Static) {
		j["resource_type"] = ResourceType::Static;
		j["resource_path"] = m_ResourcePath;
	}
	// Default case for other nodes (cameras, empty nodes, etc.)
	else {
		j["resource_type"] = m_ResourceType;
	}

	// Recursively write children
	j["nodes"] = json::array();
	for (const auto& node : m_Nodes) {
		json child_json;
		node->JWrite(child_json);
		j["nodes"].push_back(child_json);
	}
}

void GraphNode::JRead(const json& j) {
	// Read basic properties
	m_Name = j.value("name", "Node");

	if (j.contains("position")) m_Position = j.at("position").get<glm::vec3>();
	if (j.contains("scale")) m_Scale = j.at("scale").get<glm::vec3>();
	if (j.contains("rotation")) m_Rotation = j.at("rotation").get<glm::mat4>();

	m_HideFromEditor = j.value("hide_from_editor", false);

	// Read resource info and create components accordingly
	m_ResourceType = j.value("resource_type", ResourceType::SubData);

	switch (m_ResourceType) {
	case ResourceType::Static: {
		m_ResourcePath = j.value("resource_path", "");
		if (!m_ResourcePath.empty()) {
			auto imp = new Importer();
			auto r = imp->ImportEntity(m_ResourcePath.c_str());
			if (r != nullptr) {
				for (auto comp : r->GetAllComponents()) {
					AddComponent(comp);
				}
				for (auto n : r->GetNodes()) {
					AddNode(n);
				}
				delete r;
			}
			delete imp;
		}
		break;
	}
	case ResourceType::Light: {
		auto* light_comp = new LightComponent();
		if (j.contains("light_properties")) {
			const auto& light_props = j["light_properties"];
			light_comp->SetColor(light_props.value("color", glm::vec3(1.0f)));
			light_comp->SetRange(light_props.value("range", 10.0f));
			light_comp->SetIntensity(light_props.value("intensity", 1.0f));
		}
		AddComponent(light_comp);
		break;
	}
							// Add cases for Camera, Skeletal, etc. here in the future
	}


	// Recursively read children
	if (j.contains("nodes")) {
		for (const auto& child_json : j["nodes"]) {
			auto sub_node = new GraphNode();
			sub_node->JRead(child_json);
			AddNode(sub_node);
		}
	}
}

void GraphNode::JWriteScripts(json& j)  {
	std::string long_name = GetLongName();

	// --- Write C# Components (SharpComponent) ---
	auto sharp_components = GetComponents<SharpComponent>();
	if (!sharp_components.empty()) {
		json sharp_array = json::array();
		for (const auto& sc : sharp_components) {
			json sc_json;
			sc_json["class_name"] = sc->GetName();

			json fields_json = json::object();
			auto* mclass = sc->GetClass();
			if (mclass) {
				for (const auto& var : mclass->GetInstanceFields()) {
					switch (var.ctype) {
					case SHARP_TYPE_INT:
						fields_json[var.name] = mclass->GetFieldValue<int>(var.name);
						break;
					case SHARP_TYPE_FLOAT:
						fields_json[var.name] = mclass->GetFieldValue<float>(var.name);
						break;
					case SHARP_TYPE_STRING:
						fields_json[var.name] = mclass->GetFieldValue<std::string>(var.name);
						break;
					case SHARP_TYPE_VEC3: {
						auto* val = mclass->GetFieldValue<MClass*>(var.name);
						if (val) {
							fields_json[var.name] = glm::vec3(val->GetFieldValue<float>("x"), val->GetFieldValue<float>("y"), val->GetFieldValue<float>("z"));
						}
						break;
					}
										// *** ADDED: Handle GraphNode references ***
					case SHARP_TYPE_CLASS: {
						if (var.etype == "QNet.Scene.GraphNode") {
							MClass* node_instance = mclass->GetFieldValue<MClass*>(var.name);
							if (node_instance != nullptr) {
								// Call the C# GetLongName() method to get the unique identifier
								std::string node_long_name = node_instance->CallFunctionValue_String("GetLongName");
								fields_json[var.name] = { {"node_ref", node_long_name} };
							}
							else {
								fields_json[var.name] = nullptr;
							}
						}
						break;
					}
					}
				}
			}
			sc_json["fields"] = fields_json;
			sharp_array.push_back(sc_json);
		}
		j[long_name]["sharp_components"] = sharp_array;
	}

	// --- Write Python Components (ScriptComponent) ---
	auto script_components = GetComponents<ScriptComponent>();
	if (!script_components.empty()) {
		json script_array = json::array();
		for (const auto& s : script_components) {
			json s_json;
			s_json["path"] = s->GetPath();
			s_json["name"] = s->GetName();

			json vars_json = json::object();
			for (const auto& v : s->GetPythonVars()) {
				switch (v.type) {
				case VT_Int:
					vars_json[v.name] = s->GetInt(v.name);
					break;
				case VT_Float:
					vars_json[v.name] = s->GetFloat(v.name);
					break;
				case VT_String:
					vars_json[v.name] = s->GetString(v.name);
					break;
				case VT_Object:
					if (v.cls == "GraphNode") {
						auto* node = s->GetNode(v.name);
						vars_json[v.name] = (node != nullptr) ? node->GetLongName() : "null";
					}
					break;
				}
			}
			s_json["vars"] = vars_json;
			script_array.push_back(s_json);
		}
		j[long_name]["script_components"] = script_array;
	}

	// Recurse for children
	for (const auto& node : m_Nodes) {
		node->JWriteScripts(j);
	}
}

void GraphNode::JReadScripts(const json& j) {
	std::string long_name = GetLongName();

	if (j.contains(long_name)) {
		const auto& node_scripts_json = j[long_name];

		// --- Read C# Components ---
		if (node_scripts_json.contains("sharp_components")) {
			for (const auto& sc_json : node_scripts_json["sharp_components"]) {
				std::string class_name = sc_json.value("class_name", "");
				auto* cls_template = QEngine::GetMonoLib()->GetClass(class_name);
				if (cls_template) {
					auto* comp = new SharpComponent();
					AddComponent(comp);
					comp->SetClass(cls_template, QEngine::GetMonoLib()->GetAssembly(), QEngine::GetMonoLib()->GetVivid());
					comp->SetName(class_name);

					auto* cls_instance = comp->GetClass();
					if (cls_instance && sc_json.contains("fields")) {
						for (auto const& [name, val] : sc_json["fields"].items()) {
							if (val.is_number_integer()) {
								cls_instance->SetFieldValue<int>(name, val);
							}
							else if (val.is_number_float()) {
								cls_instance->SetFieldValue<float>(name, val);
							}
							else if (val.is_string()) {
								cls_instance->SetFieldValue<std::string>(name, val);
							}
							else if (val.is_array() && val.size() == 3) {
								glm::vec3 v3 = val.get<glm::vec3>();
								float nv[] = { v3.x, v3.y, v3.z };
								cls_instance->SetFieldStruct(name, nv);
							}
							// *** ADDED: Handle GraphNode references ***
							else if (val.is_object() && val.contains("node_ref")) {
								std::string node_long_name = val["node_ref"];
								GraphNode* target_node = SceneGraph::m_Instance->FindNode(node_long_name);
								if (target_node) {

									auto new_cls = comp->CreateGraphNode();

									new_cls->SetNativePtr("NodePtr",target_node);

									// A referenced node must also have a SharpComponent to get its C# instance
									//SharpComponent* target_sc = target_node->GetComponent<SharpComponent>();
									//if (target_sc) {5


										cls_instance->SetFieldClass(name, new_cls);
									//}
								}
							}
							else if (val.is_null()) {
								// Handle cases where the field was explicitly null
								cls_instance->SetFieldClass(name, nullptr);
							}
						}
					}
				}
			}
		}

		// --- Read Python Components ---
		if (node_scripts_json.contains("script_components")) {
			for (const auto& s_json : node_scripts_json["script_components"]) {
				auto* comp = new ScriptComponent();
				AddComponent(comp);
				comp->SetScript(s_json.value("path", ""), s_json.value("name", ""));

				if (s_json.contains("vars")) {
					for (auto const& [name, val] : s_json["vars"].items()) {
						if (val.is_number_integer()) comp->SetInt(name, val);
						else if (val.is_number_float()) comp->SetFloat(name, val);
						else if (val.is_string()) {
							std::string str_val = val;
							GraphNode* node_val = SceneGraph::m_Instance->FindNode(str_val);
							if (node_val) {
								comp->SetNode(name, node_val);
							}
							else {
								comp->SetString(name, str_val);
							}
						}
					}
				}
			}
		}
	}

	// Recurse for children
	for (auto& node : m_Nodes) {
		node->JReadScripts(j);
	}
}

void GraphNode::LookAt(glm::vec3 pos)
{

	glm::vec3 direction = glm::normalize(pos);

	// Define the 'up' direction for the world. Based on our previous work,
	// your engine uses Z as the vertical axis.
	glm::vec3 up = glm::vec3(0.0f,0,1.0f);

	// Use glm::quatLookAt to create a quaternion representing the desired orientation.
	// This is a safe and robust way to handle look-at rotations.
	glm::quat targetOrientation = glm::quatLookAt(direction, up);

	// Convert the quaternion to a 4x4 rotation matrix and assign it.
	m_Rotation = glm::mat4_cast(targetOrientation);

	// Mark the bounds as dirty since the node's orientation has changed.
	MarkBoundsAsDirty();

}