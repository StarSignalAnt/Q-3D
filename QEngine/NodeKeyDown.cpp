#include "NodeKeyDown.h"
#include "GameInput.h"
NodeKeyDown::NodeKeyDown() {

    SetName("Input.KeyDown");
    SetTypeName("Input.KeyDown");
    AddInput("Key", DataType::String);
    AddOutput("Pressed", DataType::Bool);


}

/// <summary>
/// **********
/// </summary>
void NodeKeyDown::CalculateOutputs()
{
    // 1. Get the string value from our "Key" input pin.
    // This value could be set by the user in the UI or by another node.
    auto keyNameOpt = GetInputValue<std::string>("Key");
    auto key = GetInputValue<std::string>("Key").value();

    bool isPressed = false;
    if (keyNameOpt && !keyNameOpt->empty()) {
        // 2. Ask your engine's input system if the specified key is pressed.
        
        if (key == "W") {
            //isPressed = GameInput::m_Key[Key::Key_W];
            //isPressed = true;
        }
        
      

    }

    // 3. Set the boolean result on our "Pressed" output pin.
    // The node that requested this data will now be able to read this value.
    if (!m_Outputs.empty()) {
        m_Outputs[0]->SetValue(isPressed);
    }
}