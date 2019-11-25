#define WIN32_LEAN_AND_MEAN

#include "Kame/Core.h"


#include <Windows.h>
#include <shellapi.h>
#include <Shlwapi.h>

#include <Kame/Platform/DirectX12/Graphics/DX12Core.h>
#include <Kame/Application/Application.h>
#include "Tutorial4.h"

#include "Kame/Logging/Log.h"

#include <dxgidebug.h>

void ReportLiveObjects()
{
    //IDXGIDebug1* dxgiDebug;
    //DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug));

    //dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_IGNORE_INTERNAL);
    //dxgiDebug->Release();
}

//int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow)
////int main(int argc, char** argv)
//{
//    int retCode = 0;
//
//    WCHAR path[MAX_PATH];
//
//    int argc = 0;
//    LPWSTR* argv = CommandLineToArgvW(lpCmdLine, &argc);
//    if (argv)
//    {
//        for (int i = 0; i < argc; ++i)
//        {
//            // -wd Specify the Working Directory.
//            if (wcscmp(argv[i], L"-wd") == 0)
//            {
//                wcscpy_s(path, argv[++i]);
//                SetCurrentDirectoryW(path);
//            }
//        }
//        LocalFree(argv);
//    }
//
//    Kame::Application::Create();
//    {
//        std::shared_ptr<Kame::Tutorial4> demo = std::make_shared<Kame::Tutorial4>(L"Learning DirectX 12 - Lesson 4", 1280, 720, true);
//        retCode = Kame::Application::Get().Run(demo);
//    }
//    Kame::Application::Destroy();
//
//    atexit(&ReportLiveObjects);
//
//    return retCode;
//}


int main(int argc, char** argv) {

  int retCode = 0;  

  Kame::Log::Init();
  KM_CORE_WARN("Initialized Log!");

  Kame::Application::Create();
  {
    std::shared_ptr<Kame::Tutorial4> demo = std::make_shared<Kame::Tutorial4>(L"Learning DirectX 12 - Lesson 4", 1280, 720, true);
    retCode = Kame::Application::Get().Run(demo);
  }
  Kame::Application::Destroy();

  atexit(&ReportLiveObjects);

  return retCode;
}