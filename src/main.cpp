#include "SinhVien.h"
#include "LinkedList.h"
#include "Bst.h"
#include "Database.h"
#include "GUI.h"

// ============================================================
// main.cpp là nơi "lắp ráp" 3 module lại với nhau.
// Cả 2 thành viên nên cùng đọc file này để hiểu luồng chạy tổng thể,
// dù chỉ trực tiếp code 1-2 module.
// ============================================================
int main() {
    // 1. Mở kết nối database, tạo bảng nếu chưa có
    sqlite3* db = moKetNoiDB("data.db");
    if (!db) return -1;

    // 2. Đọc dữ liệu đã lưu từ DB, nạp vào CẢ 2 cấu trúc dữ liệu song song:
    //    - danhSach (linked list): cấu trúc lưu trữ chính
    //    - caySV (BST): cấu trúc phụ để tìm kiếm nhanh theo Mã SV
    DanhSachSinhVien danhSach;
    CaySinhVien caySV;
    std::vector<SinhVien> dsTuDB = docTatCaSinhVien(db);
    for (const auto& sv : dsTuDB) {
        danhSach.themSinhVien(sv);
        caySV.themSinhVien(sv);
    }

    // 2b. Đọc danh mục môn học chung (dùng cho dropdown chọn môn ở GUI)
    std::vector<std::string> danhSachMonChuan = docDanhSachMonHocChuan(db);

    // 3. Khởi tạo GUI
    if (!khoiTaoGUI()) {
        dongKetNoiDB(db);
        return -1;
    }

    // 3b. Hiển thị màn hình đăng nhập trước khi vào giao diện chính
    if (!veLogin(db)) {
        // Người dùng đã đóng/thoát trong màn hình đăng nhập
        giaiPhongGUI();
        dongKetNoiDB(db);
        return 0;
    }

    // 4. Vòng lặp chính: vẽ giao diện liên tục cho tới khi người dùng thoát
    bool dangChay = true;
    while (dangChay) {
        dangChay = veKhungHinh(danhSach, caySV, db, danhSachMonChuan);
        // Việc lưu/xóa xuống SQLite giờ đã được gọi trực tiếp bên trong
        // veKhungHinh (xem GUI.cpp) mỗi khi người dùng thêm/xóa sinh viên
        // hoặc môn học.
    }

    // 5. Dọn dẹp tài nguyên
    giaiPhongGUI();
    dongKetNoiDB(db);
    return 0;
}