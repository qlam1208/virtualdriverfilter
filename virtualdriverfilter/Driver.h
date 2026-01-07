#ifndef _DRIVER_H_
#define _DRIVER_H_

// Define architecture
#ifndef AMD64
#define AMD64
#endif

#ifndef _WIN64
#define _WIN64
#endif

// Include kernel headers
#include <fltKernel.h>
#include <ntddk.h>
#include <ntstrsafe.h>

// Pool tags
#define VDRIVE_TAG 'vdrv'

// Configuration
#define SOURCE_DIRECTORY L"\\Device\\HarddiskVolume2\\SourceFolder"
#define VIRTUAL_DRIVE_LETTER L"\\DosDevices\\Z:"

// Forward declarations
typedef struct _FILTER_CONTEXT {
    PFLT_FILTER FilterHandle;
    UNICODE_STRING SourcePath;
    UNICODE_STRING VirtualPath;
} FILTER_CONTEXT, * PFILTER_CONTEXT;

// Filter callbacks
FLT_PREOP_CALLBACK_STATUS
PreCreate(
    PFLT_CALLBACK_DATA Data,
    PCFLT_RELATED_OBJECTS FltObjects,
    PVOID* CompletionContext
);

FLT_POSTOP_CALLBACK_STATUS
PostCreate(
    PFLT_CALLBACK_DATA Data,
    PCFLT_RELATED_OBJECTS FltObjects,
    PVOID CompletionContext,
    FLT_POST_OPERATION_FLAGS Flags
);

// Driver callbacks
NTSTATUS
DriverEntry(
    PDRIVER_OBJECT DriverObject,
    PUNICODE_STRING RegistryPath
);

NTSTATUS
FilterUnload(
    FLT_FILTER_UNLOAD_FLAGS Flags
);

// Global variables
extern PFLT_FILTER gFilterHandle;

#endif // _DRIVER_H_