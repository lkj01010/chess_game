#include <WinSock2.h>
#include "main.h"
#include "AppDelegate.h"
#include "cocos2d.h"

#include "GameLogi.h"

USING_NS_CC;

int APIENTRY _tWinMain(HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPTSTR    lpCmdLine,
  int       nCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);
  WSADATA wsadata;
  int ret = WSAStartup(MAKEWORD(2, 2), &wsadata);
  if (ret) { abort(); }
  // create the application instance
  AppDelegate app;
  Application::getInstance()->run();
  WSACleanup();
}
