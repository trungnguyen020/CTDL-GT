#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "SinhVien.h"
#include <vector>
#include <string>

// ============================================================
// Node của danh sách liên kết đơn
// ============================================================
struct Node {
    SinhVien data;
    Node* next;
    Node(const SinhVien& sv) : data(sv), next(nullptr) {}
};

// ============================================================
// Danh sách liên kết đơn quản lý sinh viên (cấu trúc dữ liệu chính
// theo yêu cầu đề bài). Đây là nơi lưu dữ liệu TRONG BỘ NHỚ khi
// chương trình đang chạy — song song với việc lưu bền xuống SQLite.
// Người phụ trách "logic" cài đặt các hàm này trong LinkedList.cpp
// ============================================================
class DanhSachSinhVien {
private:
    Node* head;
    int soLuong;

public:
    DanhSachSinhVien();
    ~DanhSachSinhVien();

    // Thêm sinh viên vào cuối danh sách
    void themSinhVien(const SinhVien& sv);

    // Xóa sinh viên theo mã số, trả về true nếu xóa thành công
    bool xoaSinhVien(const std::string& maSV);

    // Tìm sinh viên theo mã số, trả về nullptr nếu không tìm thấy
    SinhVien* timSinhVien(const std::string& maSV);

    // Cập nhật thông tin 1 sinh viên đã tồn tại
    bool suaSinhVien(const std::string& maSV, const SinhVien& svMoi);

    // Sắp xếp danh sách theo điểm trung bình (giảm dần) bằng merge sort.
    // Đây là lựa chọn tối ưu cho singly linked list vì O(n log n) và ổn định.
    void sapXepTheoDiemTB();

    // Trả về toàn bộ danh sách dưới dạng vector — dùng để GUI hiển thị bảng
    // (GUI không cần biết bên trong là linked list, chỉ cần vector này)
    std::vector<SinhVien> layDanhSach() const;

    // Thống kê nhanh
    int demSoLuong() const { return soLuong; }
    int demSoDau() const;
    int demSoRot() const;
    int demSoChuaCoDiem() const;
    float diemTBToanLop() const;
};

#endif // LINKEDLIST_H