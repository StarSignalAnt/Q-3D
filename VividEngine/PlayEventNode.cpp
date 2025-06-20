#include "PlayEventNode.h"
PlayEventNode::PlayEventNode() {
    // Set the display name for the node
    SetName("On Play");
    // Set the internal type name for the save/load system and registry
    SetTypeName("On Play");
    m_hasExecOut = true;
    // Use the new, cleaner API for adding ports
    
}

void PlayEventNode::FireEvent() {
    // In a real engine, this would be called by the game's main loop.
    // From here, you would get data (like delta time) and then call ExecNext()
    // to pass control down the execution chain.
    LEventNode::FireEvent();
}