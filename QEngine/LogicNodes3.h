#pragma once

#include "LGDataNode.h"
#include "LGLogicNode.h"
#include <random>
#include <sstream>
#include <string>
#include <vector>

// --- Engine Dependency Assumption ---
// The following nodes assume your engine has a static or singleton accessor for delta time.
// If not, you may need to pass delta time into your graph's execution entry point.
namespace QEngine {
    // Placeholder for where your engine's delta time would be retrieved.
    static float GetDeltaTime() { return 1.0f / 60.0f; /* e.g., 60 FPS */ }
}

// --- Logic & Comparison Nodes ---
