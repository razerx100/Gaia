#ifndef __GRAPHICS_THROW_MACROS_HPP__
#define __GRAPHICS_THROW_MACROS_HPP__

#include <GraphicsExceptions.hpp>

#define GFX_THROW_NOINFO(hr, hrCall) if(FAILED(hr = (hrCall))) throw HrException(__LINE__, __FILE__, hr)


#ifdef _DEBUG
#define DEBUG_INFO_MAN DebugInfoManager::GetDebugInfoManager()
#define GFX_THROW_NO_HR(funCall) DEBUG_INFO_MAN.Set(); funCall; std::vector<std::string> vec = DEBUG_INFO_MAN.GetMessages(); if(!vec.empty()) throw InfoException(__LINE__, __FILE__, vec)
#define GFX_THROW(hr) throw HrException(__LINE__, __FILE__, hr, DEBUG_INFO_MAN.GetMessages())
#define GFX_THROW_FAILED(hr, hrCall) DEBUG_INFO_MAN.Set(); if(FAILED(hr = (hrCall))) GFX_THROW(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) DeviceRemovedException(__LINE__, __FILE__, hr, DEBUG_INFO_MAN.GetMessages())
#else
#define GFX_THROW_NO_HR(funCall) funCall
#define GFX_THROW(hr) throw HrException(__LINE__, __FILE__, hr)
#define GFX_THROW_FAILED(hr, hrCall) GFX_THROW_NOINFO(hr, hrCall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) DeviceRemovedException(__LINE__, __FILE__, hr)
#endif

#endif