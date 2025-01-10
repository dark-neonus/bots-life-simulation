#pragma once

struct KillProtocolResponce
{
    /*
     * Indicate if kill process went okay, or not.
     * It is good practise to set this to true, if everything is okay.
     */
    bool success;
    
    KillProtocolResponce() {}
};

struct KillProtocol
{
    /// Just nothing here :)
    KillProtocol() {}
};
