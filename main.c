#include "webview/webview.h"
#include <stddef.h>
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#endif

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
  webview_set_size(w, 1000, 500, WEBVIEW_HINT_NONE);
  webview_navigate(w, "file:///C:/Ecole/ESGI/deuxieme_annee/Programmation/C/Projet_C_Domotique/index.html");
  webview_run(w);
  webview_destroy(w);
  return 0;
}