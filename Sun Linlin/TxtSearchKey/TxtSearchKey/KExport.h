#ifndef _KEXPORT_H
#define _KEXPORT_H

#ifndef _KEXPORT_DLL
#define _KEXPORT_API _declspec(dllimport)
#else
#define _KEXPORT_API _declspec(dllexport)
#endif // !_KEXPORT_DLL


#endif // !_KEXPORT_H
