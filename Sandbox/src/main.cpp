#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <shellapi.h>
#include <Shlwapi.h>

#include <Kame/Application/Application.h>
#include "Tutorial4.h"
#include "InstancedRenderingDemo.h"

#include "Kame/Logging/Log.h"

#include <Kame/Application/Entrypoint.h>

Kame::Game* CreateGame() {
  //return new Kame::Tutorial4(L"Learning DirectX 12 - Lesson 4", 1280, 720, true);
  return new InstancedRenderingDemo(L"Learning DirectX 12 - Lesson 4", 1280, 720, true);
}
