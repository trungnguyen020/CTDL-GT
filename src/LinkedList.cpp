#include "LinkedList.h"

namespace {

bool xepTruoc(const SinhVien& benTrai, const SinhVien& benPhai) {
    if (benTrai.diemTB != benPhai.diemTB) {
        return benTrai.diemTB > benPhai.diemTB;
    }
    return benTrai.maSV <= benPhai.maSV;
}

Node* hopNhatDanhSach(Node* benTrai, Node* benPhai) {
    Node* dau = nullptr;
    Node* duoi = nullptr;

    auto ganNode = [&](Node* node) {
        if (dau == nullptr) {
            dau = node;
            duoi = node;
        } else {
            duoi->next = node;
            duoi = node;
        }
    };

    while (benTrai != nullptr && benPhai != nullptr) {
        if (xepTruoc(benTrai->data, benPhai->data)) {
            Node* tiepTheo = benTrai->next;
            benTrai->next = nullptr;
            ganNode(benTrai);
            benTrai = tiepTheo;
        } else {
            Node* tiepTheo = benPhai->next;
            benPhai->next = nullptr;
            ganNode(benPhai);
            benPhai = tiepTheo;
        }
    }

    Node* conLai = (benTrai != nullptr) ? benTrai : benPhai;
    if (duoi != nullptr) {
        duoi->next = conLai;
        return dau;
    }

    return conLai;
}

Node* tachDanhSach(Node* dau) {
    if (dau == nullptr || dau->next == nullptr) {
        return nullptr;
    }

    Node* cham = dau;
    Node* nhanh = dau->next;
    while (nhanh != nullptr && nhanh->next != nullptr) {
        cham = cham->next;
        nhanh = nhanh->next->next;
    }

    Node* nuaSau = cham->next;
    cham->next = nullptr;
    return nuaSau;
}

Node* sapXepNhanh(Node* dau) {
    if (dau == nullptr || dau->next == nullptr) {
        return dau;
    }

    Node* nuaSau = tachDanhSach(dau);
    Node* nuaTruocDaSap = sapXepNhanh(dau);
    Node* nuaSauDaSap = sapXepNhanh(nuaSau);
    return hopNhatDanhSach(nuaTruocDaSap, nuaSauDaSap);
}

} // namespace

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

    if (head->data.maSV == maSV) {
        Node* temp = head;
        head = head->next;
        delete temp;
        soLuong--;
        return true;
    }

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

    return false;
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
// Sort the list by average score in descending order.
// Merge sort is the best fit for a singly linked list because it is
// O(n log n), stable, and works directly on nodes.
// ============================================================
void DanhSachSinhVien::sapXepTheoDiemTB() {
    head = sapXepNhanh(head);
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
// (chỉ tính trên các sinh viên đã có ít nhất 1 môn)
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