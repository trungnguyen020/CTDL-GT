#ifndef GUI_H
#define GUI_H

#include "LinkedList.h"
#include "Bst.h"
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
// caySV: cây nhị phân tìm kiếm chạy song song với danhSach (cùng dữ liệu,
//     nhưng tìm theo Mã SV nhanh hơn) — mọi thêm/sửa/xóa trên danhSach
//     PHẢI đồng thời gọi hàm tương ứng trên caySV để không bị lệch dữ liệu
// db: con trỏ database — dùng để đồng bộ (lưu/xóa) xuống SQLite mỗi khi
//     người dùng thêm/sửa/xóa sinh viên hoặc môn học trên giao diện
// danhSachMonChuan: danh mục tên môn học chung (dùng làm dropdown chọn môn
//     ở tab "Quản lý sinh viên"), quản lý thêm/xóa ở tab "Thêm môn học"
// Trả về false khi người dùng đóng cửa sổ (để main.cpp thoát vòng lặp)
bool veKhungHinh(DanhSachSinhVien& danhSach, CaySinhVien& caySV, sqlite3* db,
                  std::vector<std::string>& danhSachMonChuan);

// Hiển thị màn hình đăng nhập (blocking loop) — trả về true nếu đăng
// nhập thành công, false nếu người dùng đóng cửa sổ/thoát.
bool veLogin(sqlite3* db);

// Giải phóng tài nguyên GUI (gọi 1 lần trước khi chương trình kết thúc)
void giaiPhongGUI();

// ---- Các hàm vẽ từng màn hình con (nội bộ, khai báo để tham khảo cách chia nhỏ) ----
// void veFormThemSinhVien(DanhSachSinhVien& danhSach);
// void veBangDanhSach(DanhSachSinhVien& danhSach);
// void veThongKe(DanhSachSinhVien& danhSach);

#endif // GUI_H