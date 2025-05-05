#ifndef MODEREGISTRY_H
#define MODEREGISTRY_H

// standard includes
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>

// local includes
#include "Mode.h"

class ModeRegistry
{
public:
    static void registerMode(const std::string &name, Mode *mode)
    {
        modes[name] = mode;
        modeList.push_back(mode);
    }

    static Mode *getMode(const std::string &name)
    {
        auto it = modes.find(name);
        if (it != modes.end())
        {
            return it->second;
        }
        return nullptr;
    }

    static Mode *getModeByIndex(size_t index)
    {
        if (index < modeList.size())
        {
            return modeList[index];
        }
        return nullptr;
    }

    static size_t getModeCount()
    {
        return modeList.size();
    }

    static Mode *getNextMode(Mode *currentMode)
    {
        auto it = std::find(modeList.begin(), modeList.end(), currentMode);
        if (it != modeList.end())
        {
            size_t nextIndex = (std::distance(modeList.begin(), it) + 1) % modeList.size();
            return modeList[nextIndex];
        }
        return nullptr;
    }

private:
    static std::unordered_map<std::string, Mode *> modes;
    static std::vector<Mode *> modeList;
};

#endif // MODEREGISTRY_H