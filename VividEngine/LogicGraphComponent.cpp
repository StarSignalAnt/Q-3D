#include "LogicGraphComponent.h"
#include "GraphNode.h"

void LogicGraphComponent::OnUpdate(float delta) {
	if (m_Playing) {
		m_Graph->FireEvent("On Tick");
		int b = 5;
	}

}

void LogicGraphComponent::OnRender(GraphNode* cam)
{

	if (m_Playing) {
		m_Graph->FireEvent("On Render");
	}
}

void LogicGraphComponent::SetGraph(LGraph* graph) {

	m_Graph = graph;

	//auto node_var = new LGraphVariable("Node", DataType::GraphNodeRef);


//	graph->AddVariable(node_var);
	m_Name = graph->GetName();
	auto v = graph->FindVariable("Node");
	v->SetDefaultValue(m_Owner);


}

void LogicGraphComponent::OnPlay() {

	m_Playing = true;
	m_Graph->FireEvent("On Play");


}

void LogicGraphComponent::OnStop() {

	m_Playing = false;

}

std::vector<LGraphVariable*> LogicGraphComponent::GetVars() {

	return m_Graph->GetVariables();

}