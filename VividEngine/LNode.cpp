#include "LNode.h"
#include <iostream>
#include "json.hpp" 
#include "GraphNode.h"

namespace glm {
	void to_json(json& j, const glm::vec2& v) {
		j = { v.x, v.y };
	}
	void from_json(const json& j, glm::vec2& v) {
		j.at(0).get_to(v.x);
		j.at(1).get_to(v.y);
	}

	void to_json(json& j, const glm::vec3& v) {
		j = { v.x, v.y, v.z };
	}
	void from_json(const json& j, glm::vec3& v) {
		j.at(0).get_to(v.x);
		j.at(1).get_to(v.y);
		j.at(2).get_to(v.z);
	}

	void to_json(json& j, const glm::vec4& v) {
		j = { v.x, v.y, v.z, v.w };
	}
	void from_json(const json& j, glm::vec4& v) {
		j.at(0).get_to(v.x);
		j.at(1).get_to(v.y);
		j.at(2).get_to(v.z);
		j.at(3).get_to(v.w);
	}
}


LNode::LNode() : m_hasExecIn(false), m_hasExecOut(false), m_id(-1) {}

void LNode::AddInput(std::string name, DataType type) {
	int index = m_Inputs.size();
	m_Inputs.push_back(new LGInput(this, index, name, type));
}

void LNode::AddOutput(std::string name, DataType type) {
	int index = m_Outputs.size();
	m_Outputs.push_back(new LGOutput(this, index, name, type));
}


void LNode::ToJson(json& j) {
    j["id"] = m_id;
    j["typeName"] = m_typeName;
    j["name"] = m_Name;
    j["pos_x"] = m_editorPosition.x;
    j["pos_y"] = m_editorPosition.y;
    j["hasExecIn"] = m_hasExecIn;
    j["hasExecOut"] = m_hasExecOut;

    json inputs_json = json::array();
    for (const auto& input : m_Inputs) {
        if (!input->isConnected()) {
            json input_json;
            input_json["port"] = input->getPortIndex();

            const auto& val_variant = input->GetDefaultValue();
            input_json["type_index"] = val_variant.index();

            // --- BEGIN FIX ---
            // Use std::visit with a compile-time check (if constexpr) to handle
            // the special GraphNode* case.
            std::visit([&input_json](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<T, GraphNode*>) {
                    // If the type is a GraphNode pointer, save its name as a string.
                    if (arg) { // Check if the pointer is not null
                        // IMPORTANT: Replace GetName() with the actual method on your GraphNode class.
                        input_json["value"] = arg->GetLongName();
                    }
                    else {
                        input_json["value"] = ""; // Save an empty string for a null pointer
                    }
                }
                else {
                    // For all other types (int, float, string, glm::vecs), direct assignment works
                    // because they are either native JSON types or have a to_json helper function.
                    input_json["value"] = arg;
                }
                }, val_variant);
            // --- END FIX ---

            inputs_json.push_back(input_json);
        }
    }
    j["unconnected_inputs"] = inputs_json;
}
// New FromJson method
void LNode::FromJson(const json& j) {
    // ... (the first part of the function is unchanged) ...
    m_id = j.at("id");
    m_Name = j.at("name");
    m_editorPosition.x = j.at("pos_x");
    m_editorPosition.y = j.at("pos_y");
    m_hasExecIn = j.at("hasExecIn");
    m_hasExecOut = j.at("hasExecOut");

    if (j.contains("unconnected_inputs")) {
        for (const auto& input_json : j["unconnected_inputs"]) {
            int port_index = input_json.at("port");
            if (port_index < m_Inputs.size()) {
                LGInput* input = m_Inputs[port_index];
                size_t type_index = input_json.at("type_index");

                // Update the switch statement
                switch (type_index) {
                case 0: input->SetDefaultValue(LGInput::ValueVariant{ input_json.at("value").get<int>() }); break;
                case 1: input->SetDefaultValue(LGInput::ValueVariant{ input_json.at("value").get<float>() }); break;
                case 2: input->SetDefaultValue(LGInput::ValueVariant{ input_json.at("value").get<bool>() }); break; // New bool case
                case 3: input->SetDefaultValue(LGInput::ValueVariant{ input_json.at("value").get<std::string>() }); break; // Old index 2
                case 4: input->SetDefaultValue(LGInput::ValueVariant{ input_json.at("value").get<glm::vec2>() }); break; // Old index 3
                case 5: input->SetDefaultValue(LGInput::ValueVariant{ input_json.at("value").get<glm::vec3>() }); break; // Old index 4
                case 6: input->SetDefaultValue(LGInput::ValueVariant{ input_json.at("value").get<glm::vec4>() }); break; // Old index 5
                case 7: input->SetDefaultValue(LGInput::ValueVariant{ input_json.at("value").get<std::string>() }); break; // Old index 6 (for GraphNodeRef)
                }
            }
        }
    }
}

void LNode::AddExecOutput(const std::string& name) {
    auto* newPin = new LExecOutputPin();
    newPin->name = name;
    m_execOutputs.push_back(newPin);
}

void LNode::SetNextExec(int pinIndex, LNode* node) {
    if (pinIndex < m_execOutputs.size()) {
        m_execOutputs[pinIndex]->nextNode = node;
    }
}

void LNode::ExecNext(int pinIndex) {
    if (pinIndex < m_execOutputs.size() && m_execOutputs[pinIndex]->nextNode) {
        m_execOutputs[pinIndex]->nextNode->Exec();
    }
}