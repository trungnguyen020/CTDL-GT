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
    float weightGK; // fraction for midterm (e.g., 0.4)
    float weightCK; // fraction for final (e.g., 0.6)

    MonHoc() : diemGiuaKy(0), diemCuoiKy(0), diemTongKet(0), weightGK(0.4f), weightCK(0.6f) {}
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

// Xét đậu/rớt: quy tắc mẫu — TB >= 5.0 và không môn nào < 4.0
bool xetKetQua(const SinhVien& sv);

// Xét đậu/rớt cho MỘT môn học riêng lẻ (khác với xét đậu/rớt tổng thể của SV,
// dùng để hiển thị "sinh viên đậu/rớt bao nhiêu môn" ở màn hình danh sách)
bool monHocDat(const MonHoc& mon);

// Đếm số môn đã đậu / đã rớt của 1 sinh viên (dựa trên monHocDat)
int demSoMonDat(const SinhVien& sv);
int demSoMonRot(const SinhVien& sv);

// Cập nhật lại điểm tổng kết từng môn, điểm TB, và kết quả đậu/rớt
// Hàm này nên được gọi mỗi khi thêm/sửa điểm 1 môn học nào đó
void capNhatKetQua(SinhVien& sv);

#endif // SINHVIEN_H