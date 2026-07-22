#ifndef GUI_H
#define GUI_H

#include "LinkedList.h"
#include <sqlite3.h>

// ============================================================
// Module GUI — người phụ trách phần này CHỈ cần gọi các hàm công khai
// của DanhSachSinhVien (xem LinkedList.h) và Database (xem Database.h),
// KHÔNG cần biết bên trong linked list hay SQLite hoạt động ra sao.
// Cài đặt trong GUI.cpp
// ============================================================

// Khởi tạo cửa sổ + context ImGui (gọi 1 lần khi chương trình bắt đầu)
bool khoiTaoGUI();

// Vẽ 1 khung hình giao diện (gọi liên tục trong vòng lặp chính main.cpp)
// danhSach: tham chiếu tới danh sách sinh viên đang quản lý trong bộ nhớ
// db: con trỏ database — dùng để đồng bộ (lưu/xóa) xuống SQLite mỗi khi
//     người dùng thêm/sửa/xóa sinh viên trên giao diện
// Trả về false khi người dùng đóng cửa sổ (để main.cpp thoát vòng lặp)
bool veKhungHinh(DanhSachSinhVien& danhSach, sqlite3* db);

// Giải phóng tài nguyên GUI (gọi 1 lần trước khi chương trình kết thúc)
void giaiPhongGUI();

// ---- Các hàm vẽ từng màn hình con (nội bộ, khai báo để tham khảo cách chia nhỏ) ----
// void veFormThemSinhVien(DanhSachSinhVien& danhSach);
// void veBangDanhSach(DanhSachSinhVien& danhSach);
// void veThongKe(DanhSachSinhVien& danhSach);

#endif // GUI_H