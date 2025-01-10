#pragma once

#include "InitProtocol.h"
#include "UpdateProtocol.h"
#include "KillProtocol.h"

class ProtocolsHolder
{
private:
public:
    InitProtocol initProtocol;
    InitProtocolResponce initProtocolResponce;

    UpdateProtocol updateProtocol;
    UpdateProtocolResponce updateProtocolResponce;

    KillProtocol killProtocol;
    KillProtocolResponce killProtocolResponce;

    ProtocolsHolder() {}
};
