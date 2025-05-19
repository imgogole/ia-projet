#pragma once

namespace Application
{
    static bool askQuit = false;

    void Quit()
    {
        askQuit = true;
    }

    bool AskingForQuitting()
    {
        return askQuit;
    }
}