#include "LGDataNode.h"
LGDataNode::LGDataNode() {
    // Data nodes have no execution flow.
    m_hasExecIn = false;
    m_hasExecOut = false;
}

void LGDataNode::CalculateOutputs()
{
    // This function ensures the output value is always current with the input's default value.
    // This implementation assumes a 1-to-1 mapping of inputs to outputs for data nodes.
    // If an input and output share an index and type, propagate the value.

    for (int i = 0; i < m_Inputs.size(); ++i) {
        if (i < m_Outputs.size()) {
            LGInput* input = m_Inputs[i];
            LGOutput* output = m_Outputs[i];

            // If the types match, copy the default value from the input widget
            // to the output pin's value holder.
            if (input->GetType() == output->GetType()) {
                output->SetValue(input->GetDefaultValue());
            }
        }
    }
}