#include "SinhVien.h"

// ============================================================
// Tính điểm tổng kết cho 1 môn học
// Công thức mẫu: 30% giữa kỳ + 70% cuối kỳ (có thể chỉnh theo quy chế thật)
// ============================================================
float tinhDiemTongKet(const MonHoc& mon) {
    return mon.diemGiuaKy * 0.4f + mon.diemCuoiKy * 0.6f;
}

// ============================================================
// Tính điểm trung bình của tất cả các môn trong 1 sinh viên
// ============================================================
float tinhDiemTrungBinh(const SinhVien& sv) {
    if (sv.danhSachMon.empty()) {
        return 0.0f;
    }

    float tongDiem = 0.0f;
    for (const MonHoc& mon : sv.danhSachMon) {
        tongDiem += mon.diemTongKet;
    }
    return tongDiem / sv.danhSachMon.size();
}

// ============================================================
// Xét đậu/rớt theo quy chế mẫu:
//   - Điểm trung bình >= 5.0
//   - VÀ không có môn nào bị điểm liệt (< 4.0)
// (Nhóm có thể chỉnh lại quy tắc này cho đúng với quy chế thật của trường)
// ============================================================
bool xetKetQua(const SinhVien& sv) {
    if (sv.diemTB < 5.0f) {
        return false;
    }

    for (const MonHoc& mon : sv.danhSachMon) {
        if (mon.diemTongKet < 3.9f) {
            return false; // có môn điểm liệt -> rớt dù điểm TB đủ
        }
    }
    return true;
}

// ============================================================
// Xét đậu/rớt cho MỘT môn học riêng lẻ (quy tắc mẫu: >= 4.0 là đậu)
// ============================================================
bool monHocDat(const MonHoc& mon) {
    return mon.diemTongKet >= 4.0f;
}

// ============================================================
// Đếm số môn đã đậu của 1 sinh viên
// ============================================================
int demSoMonDat(const SinhVien& sv) {
    int dem = 0;
    for (const MonHoc& mon : sv.danhSachMon) {
        if (monHocDat(mon)) dem++;
    }
    return dem;
}

// ============================================================
// Đếm số môn đã rớt của 1 sinh viên
// ============================================================
int demSoMonRot(const SinhVien& sv) {
    return static_cast<int>(sv.danhSachMon.size()) - demSoMonDat(sv);
}

// ============================================================
// Cập nhật lại điểm tổng kết từng môn, điểm TB, và kết quả đậu/rớt
// Hàm này nên được gọi mỗi khi thêm/sửa điểm 1 môn học nào đó
// ============================================================
void capNhatKetQua(SinhVien& sv) {
    // Tính lại điểm tổng kết cho từng môn trước
    for (MonHoc& mon : sv.danhSachMon) {
        mon.diemTongKet = tinhDiemTongKet(mon);
    }

    // Sau đó tính điểm trung bình và xét kết quả
    sv.diemTB = tinhDiemTrungBinh(sv);
    sv.datMonHoc = xetKetQua(sv);
}