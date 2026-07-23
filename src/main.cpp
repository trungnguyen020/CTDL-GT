#include "SinhVien.h"
#include "LinkedList.h"
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

    // 2. Đọc dữ liệu đã lưu từ DB, nạp vào danh sách liên kết (bộ nhớ)
    DanhSachSinhVien danhSach;
    std::vector<SinhVien> dsTuDB = docTatCaSinhVien(db);
    for (const auto& sv : dsTuDB) {
        danhSach.themSinhVien(sv);
    }

    // 2b. Đọc danh mục môn học chung (dùng cho dropdown chọn môn ở GUI)
    std::vector<std::string> danhSachMonChuan = docDanhSachMonHocChuan(db);

    // 3. Khởi tạo GUI
    if (!khoiTaoGUI()) {
        dongKetNoiDB(db);
        return -1;
    }

    // 4. Vòng lặp chính: vẽ giao diện liên tục cho tới khi người dùng thoát
    bool dangChay = true;
    while (dangChay) {
        dangChay = veKhungHinh(danhSach, db, danhSachMonChuan);
        // Việc lưu/xóa xuống SQLite giờ đã được gọi trực tiếp bên trong
        // veKhungHinh (xem GUI.cpp) mỗi khi người dùng thêm/xóa sinh viên
        // hoặc môn học.
    }

    // 5. Dọn dẹp tài nguyên
    giaiPhongGUI();
    dongKetNoiDB(db);
    return 0;
}