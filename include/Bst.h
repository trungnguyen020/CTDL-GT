#ifndef BST_H
#define BST_H

#include "SinhVien.h"
#include <string>

// ============================================================
// Cây nhị phân tìm kiếm (BST) — sắp xếp theo Mã SV (so sánh chuỗi).
// Đây là cấu trúc dữ liệu THỨ HAI, chạy SONG SONG với danh sách liên
// kết đơn (DanhSachSinhVien ở LinkedList.h). Cả 2 cùng lưu dữ liệu
// sinh viên, nhưng BST giúp TÌM KIẾM theo Mã SV nhanh hơn — trung bình
// O(log n) thay vì O(n) như duyệt tuần tự trên linked list.
//
// QUAN TRỌNG: BST và linked list phải được đồng bộ với nhau. Bất cứ
// khi nào thêm/sửa/xóa 1 sinh viên ở DanhSachSinhVien, cũng phải gọi
// hàm tương ứng ở đây để 2 cấu trúc không bị lệch dữ liệu (xem các
// điểm gọi trong GUI.cpp).
// ============================================================

struct BSTNode {
    SinhVien data;
    BSTNode* left;
    BSTNode* right;
    BSTNode(const SinhVien& sv) : data(sv), left(nullptr), right(nullptr) {}
};

class CaySinhVien {
private:
    BSTNode* root;
    int soLuong;

    BSTNode* themDeQuy(BSTNode* node, const SinhVien& sv);
    BSTNode* timMinDeQuy(BSTNode* node);
    BSTNode* xoaDeQuy(BSTNode* node, const std::string& maSV, bool& daXoa);
    void huyDeQuy(BSTNode* node);
    int doSauDeQuy(BSTNode* node) const;

public:
    CaySinhVien();
    ~CaySinhVien();

    // Thêm 1 sinh viên vào cây (bỏ qua nếu Mã SV đã tồn tại)
    void themSinhVien(const SinhVien& sv);

    // Xóa sinh viên theo Mã SV, trả về true nếu xóa thành công
    bool xoaSinhVien(const std::string& maSV);

    // Tìm sinh viên theo Mã SV — đây là thao tác BST được tối ưu (O(log n))
    SinhVien* timSinhVien(const std::string& maSV);

    // Cập nhật (ghi đè) dữ liệu 1 sinh viên đã có trong cây — gọi hàm này
    // mỗi khi sửa tên/điểm của sinh viên ở linked list để đồng bộ sang đây
    void capNhatSinhVien(const std::string& maSV, const SinhVien& svMoi);

    int demSoLuong() const { return soLuong; }

    // Độ sâu hiện tại của cây — dùng để minh họa trong báo cáo (so sánh
    // với n để thấy rõ log n nhỏ hơn n nhiều khi n lớn)
    int doSauCay() const { return doSauDeQuy(root); }
};

#endif // BST_H