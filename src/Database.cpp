#include "Database.h"
#include <iostream>
#include <sstream>

// ============================================================
// Mở kết nối tới file database, tự tạo 2 bảng nếu chưa tồn tại:
//   - sinhvien: thông tin chung của 1 sinh viên
//   - monhoc:   các môn học của từng sinh viên (1-nhiều với sinhvien)
// ============================================================
sqlite3* moKetNoiDB(const std::string& duongDan) {
    sqlite3* db = nullptr;
    int rc = sqlite3_open(duongDan.c_str(), &db);

    if (rc != SQLITE_OK) {
        std::cerr << "Khong the mo database: " << sqlite3_errmsg(db) << std::endl;
        return nullptr;
    }

    const char* taoBangSinhVien =
        "CREATE TABLE IF NOT EXISTS sinhvien ("
        "  maSV TEXT PRIMARY KEY,"
        "  hoTen TEXT NOT NULL,"
        "  diemTB REAL DEFAULT 0,"
        "  datMonHoc INTEGER DEFAULT 0"
        ");";

    const char* taoBangMonHoc =
        "CREATE TABLE IF NOT EXISTS monhoc ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  maSV TEXT NOT NULL,"
        "  tenMon TEXT NOT NULL,"
        "  diemGiuaKy REAL DEFAULT 0,"
        "  diemCuoiKy REAL DEFAULT 0,"
        "  diemTongKet REAL DEFAULT 0,"
        "  FOREIGN KEY(maSV) REFERENCES sinhvien(maSV)"
        ");";

    const char* taoBangMonHocChuan =
        "CREATE TABLE IF NOT EXISTS monhocchuan ("
        "  tenMon TEXT PRIMARY KEY,"
        "  weight_gk REAL DEFAULT 0.4,"
        "  weight_ck REAL DEFAULT 0.6"
        ");";

    const char* taoBangTaiKhoan =
        "CREATE TABLE IF NOT EXISTS taikhoan ("
        "  tenDangNhap TEXT PRIMARY KEY,"
        "  matKhau TEXT NOT NULL"
        ");";

    char* loi = nullptr;
    sqlite3_exec(db, taoBangSinhVien, nullptr, nullptr, &loi);
    if (loi) { std::cerr << "Loi tao bang sinhvien: " << loi << std::endl; sqlite3_free(loi); }

    sqlite3_exec(db, taoBangMonHoc, nullptr, nullptr, &loi);
    if (loi) { std::cerr << "Loi tao bang monhoc: " << loi << std::endl; sqlite3_free(loi); }

    sqlite3_exec(db, taoBangMonHocChuan, nullptr, nullptr, &loi);
    if (loi) { std::cerr << "Loi tao bang monhocchuan: " << loi << std::endl; sqlite3_free(loi); }

    // Nếu người dùng cập nhật từ phiên bản cũ, cố gắng thêm cột weight nếu chưa có
    sqlite3_exec(db, "ALTER TABLE monhocchuan ADD COLUMN weight_gk REAL DEFAULT 0.4;", nullptr, nullptr, nullptr);
    sqlite3_exec(db, "ALTER TABLE monhocchuan ADD COLUMN weight_ck REAL DEFAULT 0.6;", nullptr, nullptr, nullptr);

    sqlite3_exec(db, taoBangTaiKhoan, nullptr, nullptr, &loi);
    if (loi) { std::cerr << "Loi tao bang taikhoan: " << loi << std::endl; sqlite3_free(loi); }

    // Seed 1 tai khoan giao vien mac dinh neu bang dang rong (lan chay dau tien)
    const char* demTaiKhoan = "SELECT COUNT(*) FROM taikhoan;";
    sqlite3_stmt* stmtDem = nullptr;
    sqlite3_prepare_v2(db, demTaiKhoan, -1, &stmtDem, nullptr);
    if (sqlite3_step(stmtDem) == SQLITE_ROW) {
        int soLuong = sqlite3_column_int(stmtDem, 0);
        if (soLuong == 0) {
            sqlite3_exec(db,
                "INSERT INTO taikhoan (tenDangNhap, matKhau) VALUES ('giaovien', '123456');",
                nullptr, nullptr, nullptr);
        }
    }
    sqlite3_finalize(stmtDem);

    return db;
}

// ============================================================
// Đóng kết nối database
// ============================================================
void dongKetNoiDB(sqlite3* db) {
    if (db != nullptr) {
        sqlite3_close(db);
    }
}

// ============================================================
// Hàm nội bộ: xóa hết môn học cũ của 1 sinh viên trong bảng monhoc
// (dùng khi cập nhật lại toàn bộ điểm của sinh viên đó)
// ============================================================
static void xoaMonHocTheoSV(sqlite3* db, const std::string& maSV) {
    const char* sql = "DELETE FROM monhoc WHERE maSV = ?;";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, maSV.c_str(), -1, SQLITE_STATIC);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

// ============================================================
// Hàm nội bộ: chèn toàn bộ danh sách môn học của 1 sinh viên
// ============================================================
static bool themMonHoc(sqlite3* db, const SinhVien& sv) {
    const char* sql =
        "INSERT INTO monhoc (maSV, tenMon, diemGiuaKy, diemCuoiKy, diemTongKet) "
        "VALUES (?, ?, ?, ?, ?);";

    for (const MonHoc& mon : sv.danhSachMon) {
        sqlite3_stmt* stmt = nullptr;
        sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, sv.maSV.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, mon.tenMon.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_double(stmt, 3, mon.diemGiuaKy);
        sqlite3_bind_double(stmt, 4, mon.diemCuoiKy);
        sqlite3_bind_double(stmt, 5, mon.diemTongKet);

        int rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        if (rc != SQLITE_DONE) {
            return false;
        }
    }
    return true;
}

// ============================================================
// Lưu (thêm mới) 1 sinh viên xuống DB, kèm toàn bộ môn học của sv đó
// ============================================================
bool luuSinhVien(sqlite3* db, const SinhVien& sv) {
    const char* sql =
        "INSERT INTO sinhvien (maSV, hoTen, diemTB, datMonHoc) VALUES (?, ?, ?, ?);";

    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, sv.maSV.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, sv.hoTen.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 3, sv.diemTB);
    sqlite3_bind_int(stmt, 4, sv.datMonHoc ? 1 : 0);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        std::cerr << "Loi luu sinh vien: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    return themMonHoc(db, sv);
}

// ============================================================
// Cập nhật thông tin sinh viên đã tồn tại (ghi đè thông tin chung
// + xóa hết môn học cũ rồi chèn lại danh sách môn học mới)
// ============================================================
bool capNhatSinhVienDB(sqlite3* db, const SinhVien& sv) {
    const char* sql =
        "UPDATE sinhvien SET hoTen = ?, diemTB = ?, datMonHoc = ? WHERE maSV = ?;";

    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, sv.hoTen.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 2, sv.diemTB);
    sqlite3_bind_int(stmt, 3, sv.datMonHoc ? 1 : 0);
    sqlite3_bind_text(stmt, 4, sv.maSV.c_str(), -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        std::cerr << "Loi cap nhat sinh vien: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    xoaMonHocTheoSV(db, sv.maSV);
    return themMonHoc(db, sv);
}

// ============================================================
// Xóa sinh viên khỏi DB (xóa cả các môn học liên quan)
// ============================================================
bool xoaSinhVienDB(sqlite3* db, const std::string& maSV) {
    xoaMonHocTheoSV(db, maSV);

    const char* sql = "DELETE FROM sinhvien WHERE maSV = ?;";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, maSV.c_str(), -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

// ============================================================
// Đọc toàn bộ sinh viên (kèm môn học tương ứng) từ DB
// ============================================================
std::vector<SinhVien> docTatCaSinhVien(sqlite3* db) {
    std::vector<SinhVien> ketQua;

    const char* sqlSV = "SELECT maSV, hoTen, diemTB, datMonHoc FROM sinhvien;";
    sqlite3_stmt* stmtSV = nullptr;
    sqlite3_prepare_v2(db, sqlSV, -1, &stmtSV, nullptr);

    while (sqlite3_step(stmtSV) == SQLITE_ROW) {
        SinhVien sv;
        sv.maSV = reinterpret_cast<const char*>(sqlite3_column_text(stmtSV, 0));
        sv.hoTen = reinterpret_cast<const char*>(sqlite3_column_text(stmtSV, 1));
        sv.diemTB = static_cast<float>(sqlite3_column_double(stmtSV, 2));
        sv.datMonHoc = sqlite3_column_int(stmtSV, 3) != 0;

        // Đọc danh sách môn học của sinh viên này
        const char* sqlMon =
            "SELECT tenMon, diemGiuaKy, diemCuoiKy, diemTongKet FROM monhoc WHERE maSV = ?;";
        sqlite3_stmt* stmtMon = nullptr;
        sqlite3_prepare_v2(db, sqlMon, -1, &stmtMon, nullptr);
        sqlite3_bind_text(stmtMon, 1, sv.maSV.c_str(), -1, SQLITE_STATIC);

        while (sqlite3_step(stmtMon) == SQLITE_ROW) {
            MonHoc mon;
            mon.tenMon = reinterpret_cast<const char*>(sqlite3_column_text(stmtMon, 0));
            mon.diemGiuaKy = static_cast<float>(sqlite3_column_double(stmtMon, 1));
            mon.diemCuoiKy = static_cast<float>(sqlite3_column_double(stmtMon, 2));
            mon.diemTongKet = static_cast<float>(sqlite3_column_double(stmtMon, 3));
            // Lấy hệ số cho môn này từ bảng monhocchuan nếu có
            auto hs = layHeSoMonHoc(db, mon.tenMon);
            mon.weightGK = hs.first;
            mon.weightCK = hs.second;
            // Recompute TK to ensure consistency with current weights
            mon.diemTongKet = tinhDiemTongKet(mon);
            sv.danhSachMon.push_back(mon);
        }
        sqlite3_finalize(stmtMon);

        ketQua.push_back(sv);
    }
    sqlite3_finalize(stmtSV);

    return ketQua;
}

// ============================================================
// Thêm 1 tên môn học vào danh mục chung (bỏ qua nếu đã tồn tại)
// ============================================================
bool luuMonHocChuan(sqlite3* db, const std::string& tenMon) {
    const char* sql = "INSERT OR IGNORE INTO monhocchuan (tenMon, weight_gk, weight_ck) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, tenMon.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 2, 0.4);
    sqlite3_bind_double(stmt, 3, 0.6);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

// ============================================================
// Xóa 1 tên môn học khỏi danh mục chung
// ============================================================
bool xoaMonHocChuan(sqlite3* db, const std::string& tenMon) {
    const char* sql = "DELETE FROM monhocchuan WHERE tenMon = ?;";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, tenMon.c_str(), -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

// ============================================================
// Đọc toàn bộ danh mục môn học chung, sắp xếp theo alphabet
// ============================================================
std::vector<std::string> docDanhSachMonHocChuan(sqlite3* db) {
    std::vector<std::string> ketQua;

    const char* sql = "SELECT tenMon FROM monhocchuan ORDER BY tenMon;";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        ketQua.push_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
    }
    sqlite3_finalize(stmt);

    return ketQua;
}

// ============================================================
// Kiểm tra đăng nhập: tìm tenDangNhap trong bảng, so khớp matKhau
// ============================================================
bool kiemTraDangNhap(sqlite3* db, const std::string& tenDangNhap, const std::string& matKhau) {
    const char* sql = "SELECT matKhau FROM taikhoan WHERE tenDangNhap = ?;";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, tenDangNhap.c_str(), -1, SQLITE_STATIC);

    bool hopLe = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string matKhauLuu = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        hopLe = (matKhauLuu == matKhau);
    }
    sqlite3_finalize(stmt);

    return hopLe;
}

// ============================================================
// Tạo tài khoản mới (user/password lưu thẳng vào bảng taikhoan)
// Nếu user đã tồn tại, trả về false
// ============================================================
bool taoTaiKhoan(sqlite3* db, const std::string& tenDangNhap, const std::string& matKhau) {
    const char* sql = "INSERT OR IGNORE INTO taikhoan (tenDangNhap, matKhau) VALUES (?, ?);";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, tenDangNhap.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, matKhau.c_str(), -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

std::pair<float,float> layHeSoMonHoc(sqlite3* db, const std::string& tenMon) {
    const char* sql = "SELECT weight_gk, weight_ck FROM monhocchuan WHERE tenMon = ?;";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, tenMon.c_str(), -1, SQLITE_STATIC);

    float wg = 0.4f, wc = 0.6f;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        wg = static_cast<float>(sqlite3_column_double(stmt, 0));
        wc = static_cast<float>(sqlite3_column_double(stmt, 1));
    }
    sqlite3_finalize(stmt);
    return {wg, wc};
}