#include "SinhVien.h"

// ============================================================
// Tính điểm tổng kết cho 1 môn học
// Công thức mẫu: 40% giữa kỳ + 60% cuối kỳ (có thể chỉnh theo quy chế thật)
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
    if (sv.diemTB < 4.0f) {
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