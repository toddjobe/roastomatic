#include "ModeRegistry.h"

// Static instances
std::unordered_map<std::string, Mode *> ModeRegistry::modes;
std::vector<Mode *> ModeRegistry::modeList;
