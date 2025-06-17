#include "LogicGraphComponent.h"


void LogicGraphComponent::OnUpdate(float delta) {
	m_Graph->FireEvent("On Tick");
	int b = 5;

}

void LogicGraphComponent::SetGraph(LGraph* graph) {

	m_Graph = graph;
	m_Name = graph->GetName();


}