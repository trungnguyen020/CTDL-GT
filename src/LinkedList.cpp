#include "LinkedList.h"
#include <utility> // cho std::swap

// ============================================================
// Khởi tạo danh sách rỗng
// ============================================================
DanhSachSinhVien::DanhSachSinhVien() : head(nullptr), soLuong(0) {}

// ============================================================
// Hủy toàn bộ node khi danh sách bị hủy (tránh rò rỉ bộ nhớ)
// ============================================================
DanhSachSinhVien::~DanhSachSinhVien() {
    Node* current = head;
    while (current != nullptr) {
        Node* temp = current;
        current = current->next;
        delete temp;
    }
}

// ============================================================
// Thêm sinh viên vào cuối danh sách
// ============================================================
void DanhSachSinhVien::themSinhVien(const SinhVien& sv) {
    Node* newNode = new Node(sv);

    if (head == nullptr) {
        head = newNode;
    } else {
        Node* current = head;
        while (current->next != nullptr) {
            current = current->next;
        }
        current->next = newNode;
    }
    soLuong++;
}

// ============================================================
// Xóa sinh viên theo mã số
// ============================================================
bool DanhSachSinhVien::xoaSinhVien(const std::string& maSV) {
    if (head == nullptr) return false;

    // Trường hợp xóa đúng node đầu
    if (head->data.maSV == maSV) {
        Node* temp = head;
        head = head->next;
        delete temp;
        soLuong--;
        return true;
    }

    // Trường hợp xóa node ở giữa/cuối
    Node* current = head;
    while (current->next != nullptr) {
        if (current->next->data.maSV == maSV) {
            Node* temp = current->next;
            current->next = temp->next;
            delete temp;
            soLuong--;
            return true;
        }
        current = current->next;
    }

    return false; // không tìm thấy
}

// ============================================================
// Tìm sinh viên theo mã số, trả về con trỏ tới dữ liệu để
// có thể sửa trực tiếp (hoặc nullptr nếu không tìm thấy)
// ============================================================
SinhVien* DanhSachSinhVien::timSinhVien(const std::string& maSV) {
    Node* current = head;
    while (current != nullptr) {
        if (current->data.maSV == maSV) {
            return &(current->data);
        }
        current = current->next;
    }
    return nullptr;
}

// ============================================================
// Cập nhật (ghi đè) thông tin 1 sinh viên đã tồn tại
// ============================================================
bool DanhSachSinhVien::suaSinhVien(const std::string& maSV, const SinhVien& svMoi) {
    SinhVien* sv = timSinhVien(maSV);
    if (sv == nullptr) {
        return false;
    }
    *sv = svMoi;
    return true;
}

// ============================================================
// Sắp xếp danh sách theo điểm trung bình GIẢM DẦN.
// Dùng Selection Sort trên linked list (đơn giản, dễ giải thích khi
// bảo vệ đồ án). Đổi chỗ bằng cách hoán đổi NỘI DUNG (data) giữa 2
// node thay vì hoán đổi con trỏ next -- cách này đơn giản hơn nhiều
// cho sinh viên khi trình bày báo cáo.
// ============================================================
void DanhSachSinhVien::sapXepTheoDiemTB() {
    if (head == nullptr) return;

    for (Node* i = head; i->next != nullptr; i = i->next) {
        Node* maxNode = i;
        for (Node* j = i->next; j != nullptr; j = j->next) {
            if (j->data.diemTB > maxNode->data.diemTB) {
                maxNode = j;
            }
        }
        if (maxNode != i) {
            std::swap(i->data, maxNode->data);
        }
    }
}

// ============================================================
// Chuyển toàn bộ danh sách liên kết sang vector -- dùng để
// module GUI hiển thị bảng dữ liệu mà không cần biết cấu trúc
// bên trong là linked list
// ============================================================
std::vector<SinhVien> DanhSachSinhVien::layDanhSach() const {
    std::vector<SinhVien> ketQua;
    ketQua.reserve(soLuong);

    Node* current = head;
    while (current != nullptr) {
        ketQua.push_back(current->data);
        current = current->next;
    }
    return ketQua;
}

// ============================================================
// Thống kê: đếm số sinh viên đậu (chỉ tính SV đã có ít nhất 1 môn)
// ============================================================
int DanhSachSinhVien::demSoDau() const {
    int dem = 0;
    Node* current = head;
    while (current != nullptr) {
        if (!current->data.danhSachMon.empty() && current->data.datMonHoc) dem++;
        current = current->next;
    }
    return dem;
}

// ============================================================
// Thống kê: đếm số sinh viên rớt (chỉ tính SV đã có ít nhất 1 môn)
// ============================================================
int DanhSachSinhVien::demSoRot() const {
    int dem = 0;
    Node* current = head;
    while (current != nullptr) {
        if (!current->data.danhSachMon.empty() && !current->data.datMonHoc) dem++;
        current = current->next;
    }
    return dem;
}

// ============================================================
// Thống kê: đếm số sinh viên chưa nhập điểm môn nào
// ============================================================
int DanhSachSinhVien::demSoChuaCoDiem() const {
    int dem = 0;
    Node* current = head;
    while (current != nullptr) {
        if (current->data.danhSachMon.empty()) dem++;
        current = current->next;
    }
    return dem;
}

// ============================================================
// Thống kê: điểm trung bình của toàn bộ lớp
// (chỉ tính trên các sinh viên ĐÃ có ít nhất 1 môn, tránh bị kéo
// thấp bởi sinh viên vừa thêm nhưng chưa kịp nhập điểm)
// ============================================================
float DanhSachSinhVien::diemTBToanLop() const {
    float tong = 0.0f;
    int soLuongDaCoDiem = 0;

    Node* current = head;
    while (current != nullptr) {
        if (!current->data.danhSachMon.empty()) {
            tong += current->data.diemTB;
            soLuongDaCoDiem++;
        }
        current = current->next;
    }

    if (soLuongDaCoDiem == 0) return 0.0f;
    return tong / soLuongDaCoDiem;
}