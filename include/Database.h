#ifndef DATABASE_H
#define DATABASE_H

#include "SinhVien.h"
#include <sqlite3.h>
#include <vector>
#include <string>

// ============================================================
// Module Database — người phụ trách phần này chỉ cần biết SQL cơ bản,
// không cần hiểu chi tiết linked list hay GUI.
// Cài đặt các hàm này trong Database.cpp
// ============================================================

// Mở kết nối tới file database, tự tạo bảng nếu chưa tồn tại
// Trả về con trỏ sqlite3* hoặc nullptr nếu lỗi
sqlite3* moKetNoiDB(const std::string& duongDan);

// Đóng kết nối database
void dongKetNoiDB(sqlite3* db);

// Lưu (thêm mới) 1 sinh viên xuống DB
bool luuSinhVien(sqlite3* db, const SinhVien& sv);

// Cập nhật thông tin sinh viên đã tồn tại trong DB
bool capNhatSinhVienDB(sqlite3* db, const SinhVien& sv);

// Xóa sinh viên khỏi DB theo mã số
bool xoaSinhVienDB(sqlite3* db, const std::string& maSV);

// Đọc toàn bộ danh sách sinh viên từ DB — dùng để nạp vào
// DanhSachSinhVien (linked list) lúc khởi động chương trình
std::vector<SinhVien> docTatCaSinhVien(sqlite3* db);

// ============================================================
// Quản lý danh mục môn học chung (dùng để hiển thị dropdown chọn môn
// ở tab "Quản lý sinh viên", thay vì phải gõ tay tên môn mỗi lần)
// ============================================================

// Thêm 1 tên môn học vào danh mục chung
bool luuMonHocChuan(sqlite3* db, const std::string& tenMon);

// Xóa 1 tên môn học khỏi danh mục chung
bool xoaMonHocChuan(sqlite3* db, const std::string& tenMon);

// Đọc toàn bộ danh mục môn học chung
std::vector<std::string> docDanhSachMonHocChuan(sqlite3* db);

// ============================================================
// Đăng nhập giáo viên (tài khoản duy nhất, không có vai trò sinh viên)
// ============================================================

// Kiểm tra tên đăng nhập + mật khẩu có khớp trong bảng taikhoan không.
// Lưu ý: đây là bài tập lớn nên lưu mật khẩu dạng thô (plain text) cho
// đơn giản — trong ứng dụng thực tế cần mã hóa (hash) mật khẩu.
bool kiemTraDangNhap(sqlite3* db, const std::string& tenDangNhap, const std::string& matKhau);

#endif // DATABASE_H