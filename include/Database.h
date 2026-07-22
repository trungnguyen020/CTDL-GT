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

#endif // DATABASE_H