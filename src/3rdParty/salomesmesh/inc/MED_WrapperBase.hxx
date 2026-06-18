#ifndef _MED_WrapperBase_HXX_
#define _MED_WrapperBase_HXX_

#ifdef WIN32
#if defined NETGENPlugin_EXPORTS
#define NETGENPlugin_EXPORT __declspec( dllexport )
#else
#define NETGENPlugin_EXPORT __declspec( dllimport )
#endif
#else
#define NETGENPlugin_EXPORT
#endif

#endif
