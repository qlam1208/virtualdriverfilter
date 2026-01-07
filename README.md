# Virtual Drive Filter

## 📋 Mô tả

Driver này cho phép ánh xạ một thư mục trên ổ đĩa thành một ổ đĩa ảo độc lập. Ví dụ: ánh xạ `C:\SourceFolder` thành ổ đĩa `Z:\`.

**Đặc điểm:**
- ✅ Kernel-mode minifilter driver
- ✅ Sử dụng Windows Driver Kit (WDK)
- ✅ Ánh xạ thư mục thành ổ đĩa ảo
- ✅ Hỗ trợ Windows 10/11 (x64)

## 🛠️ Yêu cầu hệ thống

### Môi trường phát triển:
- **Windows 10/11** (x64)
- **Visual Studio 2019/2022**
  - Workload: "Desktop development with C++"
- **Windows SDK** (10.0.22621.0 hoặc mới hơn)
- **Windows Driver Kit (WDK)** (phiên bản tương ứng với SDK)

### Môi trường test:
- **Máy ảo** (VMware/VirtualBox/Hyper-V) - **KHUYẾN NGHỊ**
- **Windows 10/11** với Test Signing Mode enabled

## 📦 Cài đặt môi trường phát triển

### Bước 1: Cài đặt Visual Studio

1. Tải Visual Studio 2022: https://visualstudio.microsoft.com/downloads/
2. Chọn workload **"Desktop development with C++"**
3. Cài đặt

### Bước 2: Cài đặt Windows SDK

1. Tải Windows SDK: https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/
2. Chọn phiên bản mới nhất (ví dụ: 10.0.22621.0)
3. Cài đặt với tất cả components

### Bước 3: Cài đặt Windows Driver Kit (WDK)

1. Tải WDK: https://learn.microsoft.com/en-us/windows-hardware/drivers/download-the-wdk
2. Chạy `wdksetup.exe` as Administrator
3. Chọn "Install the Windows Driver Kit"
4. Chọn tất cả components:
   - Application Verifier
   - Debugging Tools for Windows
   - Windows Driver Kit Development Tools
   - Windows Driver Kit Headers and Libraries
5. Cài đặt (có thể mất 15-30 phút)
6. Khởi động lại Visual Studio

## 🔨 Build driver

### Bước 1: Clone repository

```bash
git clone https://github.com/yourusername/virtualdriverfilter.git
cd virtualdriverfilter
```

### Bước 2: Mở project trong Visual Studio

```
File → Open → Project/Solution
Chọn: virtualdriverfilter.sln
```

### Bước 3: Cấu hình project

Đảm bảo các settings sau đã đúng:

**Configuration Properties → General:**
- Configuration Type: `Dynamic Library (.dll)`
- Target Extension: `.sys`
- Platform: `x64`

**Configuration Properties → C/C++ → General:**
- Additional Include Directories:
  ```
  C:\Program Files (x86)\Windows Kits\10\Include\{version}\km
  C:\Program Files (x86)\Windows Kits\10\Include\{version}\shared
  C:\Program Files (x86)\Windows Kits\10\Include\{version}\um
  ```

**Configuration Properties → C/C++ → Preprocessor:**
```
_AMD64_
_WIN64
_KERNEL_MODE
POOL_NX_OPTIN=1
```

**Configuration Properties → C/C++ → Code Generation:**
- Runtime Library: `Multi-threaded (/MT)`
- Basic Runtime Checks: `Default`
- Buffer Security Check: `No (/GS-)`

**Configuration Properties → Linker → General:**
- Additional Library Directories:
  ```
  C:\Program Files (x86)\Windows Kits\10\Lib\{version}\km\x64
  ```

**Configuration Properties → Linker → Input:**
- Additional Dependencies:
  ```
  ntoskrnl.lib;hal.lib;wdmsec.lib;fltmgr.lib;BufferOverflowK.lib
  ```
- Ignore Specific Default Libraries:
  ```
  msvcrt.lib;msvcrtd.lib;libcmt.lib;libcmtd.lib
  ```

**Configuration Properties → Linker → System:**
- SubSystem: `Native (/SUBSYSTEM:NATIVE)`
- Driver: `Driver (/DRIVER)`

**Configuration Properties → Linker → Advanced:**
- Entry Point: `DriverEntry`

**Configuration Properties → Linker → Manifest File:**
- Generate Manifest: `No (/MANIFEST:NO)`

### Bước 4: Build

```
Build → Rebuild Solution (Ctrl+Shift+B)
```

Output file sẽ được tạo tại:
```
x64\Debug\virtualdriverfilter.sys
```

## 📥 Cài đặt driver


### Bước 1: Chuẩn bị máy test

1. Tạo máy ảo Windows 10/11
2. Copy các file sau vào máy ảo:
   - `virtualdriverfilter.sys`
   - `virtualdriverfilter.inf`
   - `install.bat`
   - `uninstall.bat`

### Bước 2: Enable Test Signing Mode

Mở **Command Prompt as Administrator** trên máy test:

```batch
bcdedit /set testsigning on
bcdedit /set nointegritychecks on
shutdown /r /t 0
```

Máy sẽ restart. Sau khi khởi động, góc phải dưới màn hình sẽ hiển thị **"Test Mode"**.

### Bước 3: Tạo thư mục source

```batch
mkdir C:\SourceFolder
echo Test file > C:\SourceFolder\test.txt
```

### Bước 4: Cài đặt driver

**Cách 1: Dùng install script (Đơn giản nhất)**

Right-click `install.bat` → **Run as Administrator**

**Cách 2: Cài thủ công**

```batch
REM Copy driver
copy virtualdriverfilter.sys C:\Windows\System32\drivers\

REM Tạo service
sc create VirtualDriveFilter type= filesys binPath= "C:\Windows\System32\drivers\virtualdriverfilter.sys" start= demand

REM Start service
sc start VirtualDriveFilter

REM Kiểm tra status
sc query VirtualDriveFilter
```

**Cách 3: Dùng pnputil**

```batch
pnputil /add-driver virtualdriverfilter.inf /install
```

### Bước 5: Kiểm tra

Mở **File Explorer**, bạn sẽ thấy ổ đĩa **Z:\** xuất hiện.

Hoặc dùng command line:
```batch
dir Z:\
```

## 🐛 Debug

### Xem driver logs

1. Tải **DebugView**: https://learn.microsoft.com/en-us/sysinternals/downloads/debugview
2. Chạy **Dbgview.exe as Administrator**
3. Menu: **Capture** → Check **"Capture Kernel"**
4. Start driver và xem output

### Kiểm tra driver status

```batch
REM Xem service status
sc query VirtualDriveFilter

REM Xem filter instances
fltmc instances

REM Xem tất cả filters
fltmc filters
```

### Xem Event Viewer

1. Nhấn **Windows + X** → **Event Viewer**
2. **Windows Logs** → **System**
3. Tìm events từ Source: **Service Control Manager**

## 🗑️ Gỡ cài đặt

**Cách 1: Dùng uninstall script**

Right-click `uninstall.bat` → **Run as Administrator**

**Cách 2: Gỡ thủ công**

```batch
REM Stop service
sc stop VirtualDriveFilter

REM Delete service
sc delete VirtualDriveFilter

REM Xóa driver file
del C:\Windows\System32\drivers\virtualdriverfilter.sys
```

**Cách 3: Dùng pnputil**

```batch
pnputil /delete-driver virtualdriverfilter.inf /uninstall
```

## 📁 Cấu trúc project

```
virtualdriverfilter/
├── Driver.h              # Header chính, định nghĩa constants và functions
├── Driver.c              # Entry point, registration, unload
├── Operations.c          # File system operations (PreCreate, PostCreate)
├── virtualdriverfilter.inf   # Driver installation file
├── install.bat           # Script cài đặt tự động
├── uninstall.bat         # Script gỡ cài đặt tự động
├── virtualdriverfilter.vcxproj   # Visual Studio project file
├── virtualdriverfilter.sln       # Visual Studio solution
└── README.md             # Documentation
```

## 🔧 Cấu hình

### Thay đổi drive letter

Sửa trong **Driver.h**:

```c
#define VIRTUAL_DRIVE_LETTER L"\\??\\Z:"  // Đổi Z thành drive letter khác
```

### Thay đổi thư mục source

Sửa trong **Driver.h**:

```c
#define SOURCE_DIRECTORY L"\\??\\C:\\SourceFolder"  // Đổi đường dẫn
```

Sau khi sửa, rebuild driver và cài lại.

## ⚠️ Lưu ý

1. **Driver chưa được ký số (unsigned)**: Chỉ chạy được trong Test Mode
2. **Kernel driver rất nhạy cảm**: Lỗi trong code có thể gây Blue Screen
3. **Luôn test trên máy ảo**: Không bao giờ test trên máy production
4. **Backup dữ liệu**: Trước khi test driver
5. **Debug cẩn thận**: Dùng WinDbg hoặc DebugView để debug

## 🚨 Troubleshooting

### Lỗi: "Cannot verify digital signature" (Error 577)

**Giải pháp:** Enable Test Signing Mode

```batch
bcdedit /set testsigning on
shutdown /r /t 0
```

### Lỗi: Build failed - "No Target Architecture"

**Giải pháp:** Thêm preprocessor defines:

```
_AMD64_
_WIN64
_KERNEL_MODE
```

### Lỗi: "unresolved external symbol" khi link

**Giải pháp:** 
1. Thêm libraries: `ntoskrnl.lib;hal.lib;wdmsec.lib;fltmgr.lib`
2. Thêm library path: `C:\Program Files (x86)\Windows Kits\10\Lib\{version}\km\x64`

### Lỗi: Driver không start

**Kiểm tra:**
1. Test Mode đã bật chưa?
2. Xem Event Viewer để biết lỗi cụ thể
3. Dùng DebugView để xem driver logs

### Không thấy ổ đĩa Z:\

**Kiểm tra:**
1. Driver có start thành công không: `sc query VirtualDriveFilter`
2. Filter có load không: `fltmc instances`
3. Thư mục source có tồn tại không: `dir C:\SourceFolder`

## 📚 Tài liệu tham khảo

- [Windows Driver Kit Documentation](https://learn.microsoft.com/en-us/windows-hardware/drivers/)
- [File System Minifilter Drivers](https://learn.microsoft.com/en-us/windows-hardware/drivers/ifs/)
- [Developing Drivers with WDK](https://learn.microsoft.com/en-us/windows-hardware/drivers/develop/)
- [Driver Samples](https://github.com/microsoft/Windows-driver-samples)


## 🙏 Acknowledgments

- Microsoft Windows Driver Kit team
- Windows Driver Development community

---

**⚠️ Disclaimer:** Đây là project học tập/nghiên cứu. Không sử dụng trong môi trường production. Tác giả không chịu trách nhiệm về bất kỳ thiệt hại nào do việc sử dụng driver này gây ra.
