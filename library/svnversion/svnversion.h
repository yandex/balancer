#pragma once

#define PROGRAM_VERSION GetProgramSvnVersion()
#define ARCADIA_SOURCE_PATH GetArcadiaSourcePath()
#define PRINT_VERSION PrintSvnVersionAndExit(argc, argv)

#ifdef __cplusplus
#   define EXTERN_C extern "C"
#else
#   define EXTERN_C
#endif

EXTERN_C const char* GetProgramSvnVersion();
EXTERN_C const char* GetArcadiaSourcePath();
EXTERN_C int GetProgramSvnRevision();
EXTERN_C void PrintSvnVersionAndExit(int argc, char *argv[]);
