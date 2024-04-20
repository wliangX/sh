#pragma once

#ifdef _MSC_VER
#define ICCExport extern "C" __declspec(dllexport)
#else
#define ICCExport extern "C" __attribute__((visibility("default")))
#endif // _MSC_VER
