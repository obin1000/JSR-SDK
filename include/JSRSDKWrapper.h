#pragma once
#ifdef __cplusplus
extern "C" {
#endif

__declspec(dllimport) int __stdcall sdk_add(int a, int b);

__declspec(dllimport) int __stdcall LoadPlugins();


#ifdef __cplusplus
}
#endif
