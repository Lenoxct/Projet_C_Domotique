#include "webview/webview.h"
#include <stddef.h>
#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include <limits.h>

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine,
                   int nCmdShow) {
  (void)hInst;
  (void)hPrevInst;
  (void)lpCmdLine;
  (void)nCmdShow;
#else
int main(void) {
#endif
  webview_t w = webview_create(0, NULL);
  webview_set_title(w, "Logiciel domotique");
  webview_set_size(w, 1500, 650, WEBVIEW_HINT_NONE);

  char exePath[MAX_PATH];
  GetModuleFileNameA(NULL, exePath, MAX_PATH);
  char *lastSlash = strrchr(exePath, '\\');
  
  if (lastSlash != NULL) {
      *lastSlash = '\0';
  }
  char fileUrl[MAX_PATH + 16];
  snprintf(fileUrl, sizeof(fileUrl), "file:///%s/index.html", exePath);

  webview_navigate(w, fileUrl);
  webview_run(w);
  webview_destroy(w);
  return 0;
}