#pragma once

#include <SDL.h>

#define logi(...) SDL_LogInfo(0, __VA_ARGS__)
#define logw(...) SDL_LogWarn(0, __VA_ARGS__)
#define loge(...) SDL_LogError(0, __VA_ARGS__)
