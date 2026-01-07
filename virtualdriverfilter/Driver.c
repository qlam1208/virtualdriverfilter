#include "Driver.h"

PFLT_FILTER gFilterHandle = NULL;
PDEVICE_OBJECT gDeviceObject = NULL;

// Operation callbacks
const FLT_OPERATION_REGISTRATION Callbacks[] = {
    {
        IRP_MJ_CREATE,
        0,
        PreCreate,
        PostCreate
    },
    { IRP_MJ_OPERATION_END }
};

// Filter registration
const FLT_REGISTRATION FilterRegistration = {
    sizeof(FLT_REGISTRATION),
    FLT_REGISTRATION_VERSION,
    0,
    NULL,
    Callbacks,
    FilterUnload,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

NTSTATUS
DriverEntry(
    PDRIVER_OBJECT DriverObject,
    PUNICODE_STRING RegistryPath
)
{
    NTSTATUS status;
    UNICODE_STRING deviceName, symbolicLink;

    UNREFERENCED_PARAMETER(RegistryPath);

    DbgPrint("VirtualDriveFilter: Driver loading...\n");

    // ===== PHẦN 1: TẠO Ổ ĐĨA ẢO =====

    // Tạo device object
    RtlInitUnicodeString(&deviceName, L"\\Device\\VirtualDrive");

    status = IoCreateDevice(
        DriverObject,
        0,
        &deviceName,
        FILE_DEVICE_DISK,
        FILE_DEVICE_SECURE_OPEN,
        FALSE,
        &gDeviceObject
    );

    if (!NT_SUCCESS(status)) {
        DbgPrint("VirtualDriveFilter: Failed to create device: 0x%X\n", status);
        // Không return, vẫn tiếp tục với filter
    }
    else {
        // Tạo symbolic link Z:
        RtlInitUnicodeString(&symbolicLink, L"\\DosDevices\\Z:");

        status = IoCreateSymbolicLink(&symbolicLink, &deviceName);

        if (NT_SUCCESS(status)) {
            DbgPrint("VirtualDriveFilter: Created virtual drive Z:\n");

            // Set device characteristics
            gDeviceObject->Flags |= DO_DIRECT_IO;
            gDeviceObject->Characteristics |= FILE_REMOVABLE_MEDIA;
        }
        else {
            DbgPrint("VirtualDriveFilter: Failed to create symbolic link: 0x%X\n", status);
        }
    }

    // ===== PHẦN 2: ĐĂNG KÝ MINIFILTER =====

    // Register filter
    status = FltRegisterFilter(
        DriverObject,
        &FilterRegistration,
        &gFilterHandle
    );

    if (!NT_SUCCESS(status)) {
        DbgPrint("VirtualDriveFilter: Failed to register filter: 0x%X\n", status);
        // Cleanup device nếu đã tạo
        if (gDeviceObject) {
            UNICODE_STRING link;
            RtlInitUnicodeString(&link, L"\\DosDevices\\Z:");
            IoDeleteSymbolicLink(&link);
            IoDeleteDevice(gDeviceObject);
        }
        return status;
    }

    // Start filtering
    status = FltStartFiltering(gFilterHandle);

    if (!NT_SUCCESS(status)) {
        FltUnregisterFilter(gFilterHandle);
        DbgPrint("VirtualDriveFilter: Failed to start filtering: 0x%X\n", status);
        // Cleanup
        if (gDeviceObject) {
            UNICODE_STRING link;
            RtlInitUnicodeString(&link, L"\\DosDevices\\Z:");
            IoDeleteSymbolicLink(&link);
            IoDeleteDevice(gDeviceObject);
        }
        return status;
    }

    DbgPrint("VirtualDriveFilter: Driver loaded successfully\n");
    return STATUS_SUCCESS;
}

NTSTATUS
FilterUnload(
    FLT_FILTER_UNLOAD_FLAGS Flags
)
{
    UNICODE_STRING symbolicLink;

    UNREFERENCED_PARAMETER(Flags);

    DbgPrint("VirtualDriveFilter: Unloading driver...\n");

    // Xóa symbolic link và device
    if (gDeviceObject) {
        RtlInitUnicodeString(&symbolicLink, L"\\DosDevices\\Z:");
        IoDeleteSymbolicLink(&symbolicLink);
        IoDeleteDevice(gDeviceObject);
        DbgPrint("VirtualDriveFilter: Deleted virtual drive Z:\n");
    }

    // Unregister filter
    if (gFilterHandle != NULL) {
        FltUnregisterFilter(gFilterHandle);
    }

    DbgPrint("VirtualDriveFilter: Driver unloaded\n");
    return STATUS_SUCCESS;
}