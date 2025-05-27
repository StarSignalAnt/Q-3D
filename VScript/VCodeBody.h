#pragma once
#include "VAction.h"
#include <vector>

class VVar;

class VCodeBody :
    public VAction
{
public:

    void AddCode(VAction* action);
    std::vector<VAction*> GetCode() {
        return m_Code;
    }
    VVar* Exec();
    void SetContext(VContext* context);
    std::vector<VAction*> GetFor() {
        return m_Code;
    }
private:

    std::vector<VAction*> m_Code;

};

