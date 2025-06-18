#include "NodeAddFloat.h"
NodeAddFloat::NodeAddFloat()
    : NodeAddBase<float>(DataType::Float, "Add (Float)", "AddFloatNode")
{
    // That's it! The base template handles all the work.
}