#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <shellapi.h>
#include <Shlwapi.h>

#include <Kame/Application/Application.h>
#include "Tutorial4.h"

#include "Kame/Logging/Log.h"

#include <Kame/Application/Entrypoint.h>

Kame::Game* CreateGame() {
  return new Kame::Tutorial4(L"Learning DirectX 12 - Lesson 4", 1280, 720, true);
}

//int main(int argc, char** argv) {
//
//  int retCode = 0;  
//
//  Kame::Log::Init();
//  KM_CORE_WARN("Initialized Log!");
//
//  Kame::Application::Create();
//  {
//    std::shared_ptr<Kame::Tutorial4> demo = std::make_shared<Kame::Tutorial4>(L"Learning DirectX 12 - Lesson 4", 1280, 720, true);
//    retCode = Kame::Application::Get().Run(demo);
//  }
//  Kame::Application::Destroy();
//
//  atexit(&Kame::Application::ReportLiveObjects);
//
//  return retCode;
//}