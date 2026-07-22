#ifndef SINHVIEN_H
#define SINHVIEN_H

#include <string>
#include <vector>

// ============================================================
// Struct dữ liệu môn học
// ============================================================
struct MonHoc {
    std::string tenMon;
    float diemGiuaKy;
    float diemCuoiKy;
    float diemTongKet;   // = diemGiuaKy * 0.3 + diemCuoiKy * 0.7 (tùy quy chế)

    MonHoc() : diemGiuaKy(0), diemCuoiKy(0), diemTongKet(0) {}
};

// ============================================================
// Struct dữ liệu sinh viên
// ============================================================
struct SinhVien {
    std::string maSV;
    std::string hoTen;
    std::vector<MonHoc> danhSachMon;
    float diemTB;        // điểm trung bình các môn
    bool datMonHoc;       // true = Đậu, false = Rớt

    SinhVien() : diemTB(0), datMonHoc(false) {}
};

// ============================================================
// Các hàm xử lý nghiệp vụ thuần túy (không đụng GUI, không đụng DB)
// Người phụ trách "logic" sẽ cài đặt các hàm này trong SinhVien.cpp
// ============================================================

// Tính điểm tổng kết cho 1 môn học dựa trên điểm giữa kỳ + cuối kỳ
float tinhDiemTongKet(const MonHoc& mon);

// Tính điểm trung bình của toàn bộ các môn trong 1 sinh viên
float tinhDiemTrungBinh(const SinhVien& sv);

// Xét đậu/rớt: quy tắc mẫu — TB >= 4.0 và không môn nào < 3.9
bool xetKetQua(const SinhVien& sv);

// Cập nhật lại diemTB và datMonHoc cho 1 sinh viên (gọi sau khi thêm/sửa điểm)
void capNhatKetQua(SinhVien& sv);

#endif // SINHVIEN_H