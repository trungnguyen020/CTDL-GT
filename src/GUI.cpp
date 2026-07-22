#include "GUI.h"
#include "SinhVien.h"
#include "Database.h"

#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <cstring>
#include <string>
#include <vector>

static GLFWwindow* window = nullptr;

// ============================================================
// Khởi tạo cửa sổ GLFW + context ImGui
// ============================================================
bool khoiTaoGUI() {
    if (!glfwInit()) {
        return false;
    }

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(1000, 700, "Quan Ly Diem Sinh Vien", nullptr, nullptr);
    if (window == nullptr) {
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // bật vsync

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    return true;
}

// ============================================================
// Vẽ 1 khung hình giao diện — được gọi liên tục trong vòng lặp chính
// Trả về false khi người dùng đóng cửa sổ
// ============================================================
bool veKhungHinh(DanhSachSinhVien& danhSach, sqlite3* db) {
    if (glfwWindowShouldClose(window)) {
        return false;
    }

    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // ---- Buffer nhập liệu (giữ trạng thái giữa các khung hình) ----
    static char bufMaSV[32] = "";
    static char bufHoTen[128] = "";
    static std::string thongBaoSV = "";

    static int svDangChon = -1; // index trong vector layDanhSach() đang chọn để nhập điểm
    static char bufTenMon[64] = "";
    static float diemGiuaKy = 0.0f;
    static float diemCuoiKy = 0.0f;
    static std::string thongBaoDiem = "";

    static char bufTimKiem[32] = "";

    ImGui::SetNextWindowSize(ImVec2(950, 700), ImGuiCond_FirstUseEver);
    ImGui::Begin("Quan ly diem sinh vien");

    // ============================================================
    // KHU VỰC 1: THÊM SINH VIÊN MỚI (chưa cần điểm, lưu ngay)
    // ============================================================
    if (ImGui::CollapsingHeader("1. Them sinh vien moi", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::InputText("Ma sinh vien##sv", bufMaSV, IM_ARRAYSIZE(bufMaSV));
        ImGui::InputText("Ho ten##sv", bufHoTen, IM_ARRAYSIZE(bufHoTen));

        if (ImGui::Button("Them sinh vien", ImVec2(150, 30))) {
            if (strlen(bufMaSV) == 0 || strlen(bufHoTen) == 0) {
                thongBaoSV = "Loi: chua nhap Ma SV hoac Ho ten";
            } else if (danhSach.timSinhVien(bufMaSV) != nullptr) {
                thongBaoSV = "Loi: Ma SV nay da ton tai";
            } else {
                SinhVien sv;
                sv.maSV = bufMaSV;
                sv.hoTen = bufHoTen;
                // Chưa có môn học nào -> diemTB = 0, datMonHoc mặc định false
                // (sẽ hiển thị rõ là "Chua co diem" ở bảng bên dưới, không
                // nhầm thành "Rot" cho tới khi thực sự nhập điểm)

                danhSach.themSinhVien(sv);
                luuSinhVien(db, sv); // đồng bộ xuống SQLite ngay

                thongBaoSV = "Da them sinh vien " + sv.maSV + ". Nhap diem o muc 2 ben duoi.";

                bufMaSV[0] = '\0';
                bufHoTen[0] = '\0';
            }
        }

        if (!thongBaoSV.empty()) {
            ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "%s", thongBaoSV.c_str());
        }
    }

    ImGui::Separator();

    // ============================================================
    // KHU VỰC 2: NHẬP ĐIỂM CHO SINH VIÊN ĐÃ CÓ TRONG DANH SÁCH
    // ============================================================
    if (ImGui::CollapsingHeader("2. Nhap diem mon hoc", ImGuiTreeNodeFlags_DefaultOpen)) {
        std::vector<SinhVien> dsChonDiem = danhSach.layDanhSach();

        std::string nhanChon = "-- Chon sinh vien --";
        if (svDangChon >= 0 && svDangChon < (int)dsChonDiem.size()) {
            nhanChon = dsChonDiem[svDangChon].maSV + " - " + dsChonDiem[svDangChon].hoTen;
        }

        if (ImGui::BeginCombo("Chon sinh vien", nhanChon.c_str())) {
            for (int i = 0; i < (int)dsChonDiem.size(); i++) {
                bool dangChon = (svDangChon == i);
                std::string dong = dsChonDiem[i].maSV + " - " + dsChonDiem[i].hoTen;
                if (ImGui::Selectable(dong.c_str(), dangChon)) {
                    svDangChon = i;
                }
            }
            ImGui::EndCombo();
        }

        if (svDangChon >= 0 && svDangChon < (int)dsChonDiem.size()) {
            ImGui::InputText("Ten mon##diem", bufTenMon, IM_ARRAYSIZE(bufTenMon));
            ImGui::InputFloat("Diem giua ky##diem", &diemGiuaKy, 0.5f);
            ImGui::InputFloat("Diem cuoi ky##diem", &diemCuoiKy, 0.5f);

            if (ImGui::Button("Them diem mon nay", ImVec2(180, 30))) {
                if (strlen(bufTenMon) == 0) {
                    thongBaoDiem = "Loi: chua nhap ten mon";
                } else {
                    std::string maSVdangChon = dsChonDiem[svDangChon].maSV;
                    SinhVien* sv = danhSach.timSinhVien(maSVdangChon);

                    if (sv != nullptr) {
                        MonHoc mon;
                        mon.tenMon = bufTenMon;
                        mon.diemGiuaKy = diemGiuaKy;
                        mon.diemCuoiKy = diemCuoiKy;
                        sv->danhSachMon.push_back(mon);
                        capNhatKetQua(*sv);

                        capNhatSinhVienDB(db, *sv); // đồng bộ xuống SQLite

                        thongBaoDiem = "Da them mon " + mon.tenMon + " cho " + sv->maSV;

                        bufTenMon[0] = '\0';
                        diemGiuaKy = 0.0f;
                        diemCuoiKy = 0.0f;
                    }
                }
            }

            // Hiển thị các môn đã có sẵn của sinh viên đang chọn
            SinhVien* svXem = danhSach.timSinhVien(dsChonDiem[svDangChon].maSV);
            if (svXem != nullptr && !svXem->danhSachMon.empty()) {
                ImGui::Text("Cac mon da nhap:");
                for (const MonHoc& mon : svXem->danhSachMon) {
                    ImGui::BulletText("%s - GK: %.1f, CK: %.1f, TK: %.2f",
                        mon.tenMon.c_str(), mon.diemGiuaKy, mon.diemCuoiKy, mon.diemTongKet);
                }
            }
        } else {
            ImGui::TextDisabled("Chon 1 sinh vien o tren de bat dau nhap diem.");
        }

        if (!thongBaoDiem.empty()) {
            ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "%s", thongBaoDiem.c_str());
        }
    }

    ImGui::Separator();

    // ============================================================
    // KHU VỰC 3: TÌM KIẾM + SẮP XẾP + THỐNG KÊ
    // ============================================================
    ImGui::Text("3. Danh sach sinh vien");
    ImGui::InputText("Tim theo Ma SV", bufTimKiem, IM_ARRAYSIZE(bufTimKiem));
    ImGui::SameLine();
    if (ImGui::Button("Sap xep theo diem TB (giam dan)")) {
        danhSach.sapXepTheoDiemTB();
    }

    ImGui::Text("Tong so: %d | Dau: %d | Rot: %d | Chua co diem: %d | Diem TB toan lop: %.2f",
        danhSach.demSoLuong(), danhSach.demSoDau(), danhSach.demSoRot(),
        danhSach.demSoChuaCoDiem(), danhSach.diemTBToanLop());

    ImGui::Separator();

    // ============================================================
    // KHU VỰC 3: BẢNG DANH SÁCH SINH VIÊN
    // ============================================================
    std::vector<SinhVien> ds = danhSach.layDanhSach();
    std::string tuKhoa = bufTimKiem;

    if (ImGui::BeginTable("BangSinhVien", 6,
            ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY,
            ImVec2(0, 300))) {

        ImGui::TableSetupColumn("Ma SV");
        ImGui::TableSetupColumn("Ho ten");
        ImGui::TableSetupColumn("So mon");
        ImGui::TableSetupColumn("Diem TB");
        ImGui::TableSetupColumn("Ket qua");
        ImGui::TableSetupColumn("Hanh dong");
        ImGui::TableHeadersRow();

        for (const SinhVien& sv : ds) {
            // Lọc theo từ khóa tìm kiếm nếu có nhập
            if (!tuKhoa.empty() && sv.maSV.find(tuKhoa) == std::string::npos) {
                continue;
            }

            ImGui::TableNextRow();
            ImGui::TableNextColumn(); ImGui::Text("%s", sv.maSV.c_str());
            ImGui::TableNextColumn(); ImGui::Text("%s", sv.hoTen.c_str());
            ImGui::TableNextColumn(); ImGui::Text("%d", (int)sv.danhSachMon.size());
            ImGui::TableNextColumn(); ImGui::Text("%.2f", sv.diemTB);
            ImGui::TableNextColumn();
            if (sv.danhSachMon.empty()) {
                ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.3f, 1.0f), "Chua co diem");
            } else if (sv.datMonHoc) {
                ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "Dau");
            } else {
                ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Rot");
            }
            ImGui::TableNextColumn();
            ImGui::PushID(sv.maSV.c_str());
            if (ImGui::SmallButton("Xoa")) {
                danhSach.xoaSinhVien(sv.maSV);
                xoaSinhVienDB(db, sv.maSV); // đồng bộ xuống SQLite
            }
            ImGui::PopID();
        }

        ImGui::EndTable();
    }

    ImGui::End();

    // ---- Render ----
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.12f, 0.12f, 0.14f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);

    return true;
}

// ============================================================
// Giải phóng tài nguyên GUI
// ============================================================
void giaiPhongGUI() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (window != nullptr) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
}