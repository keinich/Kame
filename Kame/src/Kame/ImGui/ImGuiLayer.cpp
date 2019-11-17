#include "kmpch.h"
//#include "ImGuiLayer.h"
//
//#include "imgui.h"
//
//#ifdef KAME_PLATFORM_OPENGL
//#include "Kame/Platform/OpenGl/ImGuiOpenGLRenderer.h"
//#else
//#include "Kame/Platform/DirectX12/ImGui/ImGuiDX12Renderer.h"
//#endif
//
//#include "Kame/Application.h"
//
//// TEMPORARY
//#include <GLFW/glfw3.h>
//#include <glad/glad.h>
//
//namespace Kame {
//
//  ImGuiLayer::ImGuiLayer()
//    :Layer("ImGuiLayer") {}
//
//  ImGuiLayer::~ImGuiLayer() {}
//
//  void ImGuiLayer::OnAttach() {
//    ImGui::CreateContext();
//    ImGui::StyleColorsDark();
//
//    ImGuiIO& io = ImGui::GetIO();
//    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
//    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
//
//    // TEMPORARY: should user Kame key codes
//    io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
//    io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
//    io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
//    io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
//    io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
//    io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
//    io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
//    io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
//    io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
//    io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
//    io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
//    io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
//    io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
//    io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
//    io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
//    io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
//    io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
//    io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
//    io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
//    io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
//    io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;
//
//#ifdef KAME_PLATFORM_OPENGL
//    ImGui_ImplOpenGL3_Init("#version 410");
//#else
//    //ImGui_ImplDX12_Init(;
//#endif
//  }
//
//  void ImGuiLayer::OnDetach() {}
//
//  void ImGuiLayer::OnUpdate() {
//
//    ImGuiIO& io = ImGui::GetIO();
//    Application& app = Application::Get();
//    io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());
//
//    float time = (float)glfwGetTime();
//    io.DeltaTime = _Time > 0.0f ? (time - _Time) : (1.0f / 60.0f);
//    _Time = time;
//
//#ifdef KAME_PLATFORM_OPENGL
//    ImGui_ImplOpenGL3_NewFrame();
//#else
//    ImGui_ImplDX12_NewFrame();
//#endif
//
//    ImGui::NewFrame();
//
//    static bool show = true;
//    ImGui::ShowDemoWindow(&show);
//
//    ImGui::Render();
//#ifdef KAME_PLATFORM_OPENGL
//    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//#else
//    //ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(),);
//#endif
//  }
//
//  void ImGuiLayer::OnEvent(Event& event) {
//    EventDispatcher dispatcher(event);
//    dispatcher.Dispatch< MouseButtonPressedEvent>(KM_BIND_EVENT_FN(ImGuiLayer::OnMouseButtonPressedEvent));
//    dispatcher.Dispatch< MouseButtonReleasedEvent>(KM_BIND_EVENT_FN(ImGuiLayer::OnMouseButtonReleasedEvent));
//    dispatcher.Dispatch< MouseMovedEvent>(KM_BIND_EVENT_FN(ImGuiLayer::OnMouseMovedEvent));
//    dispatcher.Dispatch< MouseScrolledEvent>(KM_BIND_EVENT_FN(ImGuiLayer::OnMouseScrolledEvent));
//    dispatcher.Dispatch< KeyPressedEvent>(KM_BIND_EVENT_FN(ImGuiLayer::OnKeyPressedEvent));
//    dispatcher.Dispatch< KeyReleasedEvent>(KM_BIND_EVENT_FN(ImGuiLayer::OnKeyReleasedEvent));
//    dispatcher.Dispatch< KeyTypedEvent>(KM_BIND_EVENT_FN(ImGuiLayer::OnKeyTypedEvent));
//    dispatcher.Dispatch< WindowResizedEvent>(KM_BIND_EVENT_FN(ImGuiLayer::OnWindowResizedEvent));
//  }
//
//  bool ImGuiLayer::OnMouseButtonPressedEvent(MouseButtonPressedEvent& e) {
//    ImGuiIO& io = ImGui::GetIO();
//    io.MouseDown[e.GetMouseButton()] = true;
//
//    return false;
//  }
//
//  bool ImGuiLayer::OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e) {
//    ImGuiIO& io = ImGui::GetIO();
//    io.MouseDown[e.GetMouseButton()] = false;
//
//    return false;
//  }
//
//  bool ImGuiLayer::OnMouseMovedEvent(MouseMovedEvent& e) {
//    ImGuiIO& io = ImGui::GetIO();
//    io.MousePos = ImVec2(e.GetX(), e.GetY());
//
//    return false;
//  }
//
//  bool ImGuiLayer::OnMouseScrolledEvent(MouseScrolledEvent& e) {
//    ImGuiIO& io = ImGui::GetIO();
//    io.MouseWheelH += e.GetXOffset();
//    io.MouseWheel += e.GetYOffset();
//
//    return false;
//  }
//
//  bool ImGuiLayer::OnKeyPressedEvent(KeyPressedEvent& e) {
//    ImGuiIO& io = ImGui::GetIO();
//    io.KeysDown[e.GetKeyCode()] = true;
//
//    io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
//    io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
//    io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
//    io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
//
//    return false;
//  }
//
//  bool ImGuiLayer::OnKeyReleasedEvent(KeyReleasedEvent& e) {
//    ImGuiIO& io = ImGui::GetIO();
//    io.KeysDown[e.GetKeyCode()] = false;
//
//    return false;
//  }
//
//  bool ImGuiLayer::OnKeyTypedEvent(KeyTypedEvent& e) {
//    ImGuiIO& io = ImGui::GetIO();
//    int keycode = e.GetKeyCode();
//
//    if (keycode > 0 && keycode < 0x10000)
//      io.AddInputCharacter((unsigned short)keycode);
//
//    return false;
//  }
//
//  bool ImGuiLayer::OnWindowResizedEvent(WindowResizedEvent& e) {
//    ImGuiIO& io = ImGui::GetIO();
//    io.DisplaySize = ImVec2(e.GetWidth(), e.GetHeight());
//    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
//    glViewport(0, 0, e.GetWidth(), e.GetHeight());
//
//    return false;
//  }
//
//}
