#include "Bst.h"

CaySinhVien::CaySinhVien() : root(nullptr), soLuong(0) {}

CaySinhVien::~CaySinhVien() {
    huyDeQuy(root);
}

void CaySinhVien::huyDeQuy(BSTNode* node) {
    if (node == nullptr) return;
    huyDeQuy(node->left);
    huyDeQuy(node->right);
    delete node;
}

// ============================================================
// Thêm đệ quy: so sánh Mã SV để quyết định đi trái/phải, bỏ qua
// nếu Mã SV đã tồn tại (tránh trùng)
// ============================================================
BSTNode* CaySinhVien::themDeQuy(BSTNode* node, const SinhVien& sv) {
    if (node == nullptr) {
        soLuong++;
        return new BSTNode(sv);
    }
    if (sv.maSV < node->data.maSV) {
        node->left = themDeQuy(node->left, sv);
    } else if (sv.maSV > node->data.maSV) {
        node->right = themDeQuy(node->right, sv);
    }
    return node;
}

void CaySinhVien::themSinhVien(const SinhVien& sv) {
    root = themDeQuy(root, sv);
}

// ============================================================
// Tìm kiếm: đi trái/phải theo so sánh Mã SV, trung bình O(log n)
// ============================================================
SinhVien* CaySinhVien::timSinhVien(const std::string& maSV) {
    BSTNode* current = root;
    while (current != nullptr) {
        if (maSV == current->data.maSV) {
            return &(current->data);
        } else if (maSV < current->data.maSV) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    return nullptr;
}

void CaySinhVien::capNhatSinhVien(const std::string& maSV, const SinhVien& svMoi) {
    SinhVien* sv = timSinhVien(maSV);
    if (sv != nullptr) {
        *sv = svMoi;
    }
}

BSTNode* CaySinhVien::timMinDeQuy(BSTNode* node) {
    while (node->left != nullptr) {
        node = node->left;
    }
    return node;
}

// ============================================================
// Xóa đệ quy - xử lý đủ 3 trường hợp kinh điển của BST:
//   1. Node lá (không con)
//   2. Node có đúng 1 con
//   3. Node có 2 con -> thay bằng phần tử nhỏ nhất bên cây con phải
// ============================================================
BSTNode* CaySinhVien::xoaDeQuy(BSTNode* node, const std::string& maSV, bool& daXoa) {
    if (node == nullptr) return nullptr;

    if (maSV < node->data.maSV) {
        node->left = xoaDeQuy(node->left, maSV, daXoa);
    } else if (maSV > node->data.maSV) {
        node->right = xoaDeQuy(node->right, maSV, daXoa);
    } else {
        daXoa = true;

        if (node->left == nullptr && node->right == nullptr) {
            delete node;
            return nullptr;
        }
        if (node->left == nullptr) {
            BSTNode* temp = node->right;
            delete node;
            return temp;
        }
        if (node->right == nullptr) {
            BSTNode* temp = node->left;
            delete node;
            return temp;
        }

        BSTNode* ketNoi = timMinDeQuy(node->right);
        node->data = ketNoi->data;
        bool daXoaPhu = false;
        node->right = xoaDeQuy(node->right, ketNoi->data.maSV, daXoaPhu);
    }
    return node;
}

bool CaySinhVien::xoaSinhVien(const std::string& maSV) {
    bool daXoa = false;
    root = xoaDeQuy(root, maSV, daXoa);
    if (daXoa) soLuong--;
    return daXoa;
}

int CaySinhVien::doSauDeQuy(BSTNode* node) const {
    if (node == nullptr) return 0;
    int trai = doSauDeQuy(node->left);
    int phai = doSauDeQuy(node->right);
    return 1 + (trai > phai ? trai : phai);
}