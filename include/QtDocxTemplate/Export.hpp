#pragma once

#if defined(_WIN32) && !defined(__MINGW32__)
  #if defined(QTDOCTEMPLATE_BUILD_LIB)
    #define QTDOCTXTEMPLATE_EXPORT __declspec(dllexport)
  #else
    #define QTDOCTXTEMPLATE_EXPORT __declspec(dllimport)
  #endif
#else
  #define QTDOCTXTEMPLATE_EXPORT __attribute__((visibility("default")))
#endif
