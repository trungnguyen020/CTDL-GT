# Hướng dẫn cài đặt và chạy project

Toàn bộ thư viện (ImGui, GLFW, SQLite) sẽ được **CMake tự động tải về** khi
build lần đầu — không cần vào từng trang web để tải thủ công.

## Bước 1: Cài 2 công cụ nền tảng (chỉ cài 1 lần)

1. **CMake**: tải tại https://cmake.org/download/ → chọn bản Windows x64
   Installer → khi cài nhớ tick **"Add CMake to system PATH"**.
2. **Trình biên dịch C++**:
   - Windows: cài **MinGW-w64** qua https://www.msys2.org/ (làm theo hướng
     dẫn trên trang, sau khi cài chạy `pacman -S mingw-w64-ucrt-x86_64-gcc`)
   - Hoặc đơn giản hơn: cài **Visual Studio Community** (chọn mục
     "Desktop development with C++" lúc cài) — không cần dùng IDE này,
     chỉ cần lấy trình biên dịch MSVC đi kèm.
3. **Git**: tải tại https://git-scm.com/downloads (cần để CMake tự tải
   thư viện về từ GitHub).

> Lưu ý: máy nào cũng chỉ cần 3 công cụ trên, không cần cài Qt, không cần
> cài GLFW, không cần cài ImGui riêng lẻ.

## Bước 2: Clone project về máy

```bash
git clone <link-github-cua-nhom>
cd QuanLyDiem
```

## Bước 3: Build project (tự động tải thư viện trong bước này)

```bash
cmake -S . -B build
cmake --build build
```

Lần build đầu tiên sẽ mất vài phút vì CMake đang tải GLFW, ImGui, SQLite
từ GitHub về — chỉ cần internet ổn định, không cần thao tác gì thêm.
Các lần build sau sẽ nhanh hơn nhiều vì thư viện đã có sẵn trong thư mục
`build/_deps`.

## Bước 4: Chạy chương trình

```bash
# Windows
.\build\Debug\QuanLyDiem.exe

# Linux/macOS
./build/QuanLyDiem
```

## Nếu dùng VS Code

1. Cài extension **CMake Tools** (Microsoft).
2. Mở thư mục project trong VS Code.
3. Nhấn `Ctrl+Shift+P` → gõ "CMake: Configure" → chọn kit (compiler)
   đã cài ở Bước 1.
4. Nhấn nút **Build** (thanh trạng thái dưới cùng) hoặc `F7`.
5. Nhấn nút **Run** (biểu tượng tam giác) để chạy thử.
