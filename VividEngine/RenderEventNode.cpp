#include "RenderEventNode.h"

RenderEventNode::RenderEventNode() {
    // Set the display name for the node
    SetName("On Render");
    // Set the internal type name for the save/load system and registry
    SetTypeName("On Render");
    m_hasExecOut = true;
    // Use the new, cleaner API for adding ports

}

void RenderEventNode::FireEvent() {
    // In a real engine, this would be called by the game's main loop.
    // From here, you would get data (like delta time) and then call ExecNext()
    // to pass control down the execution chain.
    LEventNode::FireEvent();
}