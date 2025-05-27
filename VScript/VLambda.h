#pragma once
#include "VAction.h"

class VCodeBody;
class VScope;

class VLambda :
    public VAction
{
public:

    void SetBody(VCodeBody* body);
    VCodeBody* GetBody();
    VVar* Exec();
    void SetScope(VScope* scope) {
        m_LocalScope = scope;

    }
    VScope* GetScope() {
        return m_LocalScope;
    }
private:

    VCodeBody* m_Code;
    VScope* m_LocalScope = nullptr;

};

