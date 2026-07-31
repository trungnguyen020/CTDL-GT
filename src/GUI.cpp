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
#include <algorithm>

static GLFWwindow* window = nullptr;
// Hiện tại user đăng nhập và role (teacher nếu không phải maSV)
static std::string g_currentUser = "";
static bool g_isTeacher = false;

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

    window = glfwCreateWindow(1050, 750, "Quan Ly Diem Sinh Vien", nullptr, nullptr);
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
// Màn hình đăng nhập giáo viên (đơn giản, blocking trong vòng lặp
// ImGui cho tới khi đăng nhập thành công hoặc người dùng đóng cửa sổ)
// ============================================================
bool veLogin(sqlite3* db) {
    static char bufUser[64] = "";
    static char bufPass[64] = "";
    static std::string thongBao = "";

    while (true) {
        if (glfwWindowShouldClose(window)) return false;

        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Giữ cửa sổ đăng nhập nhỏ, căn giữa
        ImGui::SetNextWindowSize(ImVec2(400, 180), ImGuiCond_FirstUseEver);
        ImGui::Begin("Dang nhap giao vien", nullptr, ImGuiWindowFlags_NoResize);

        ImGui::InputText("Ten dang nhap", bufUser, IM_ARRAYSIZE(bufUser));
        ImGui::InputText("Mat khau", bufPass, IM_ARRAYSIZE(bufPass), ImGuiInputTextFlags_Password);

        if (ImGui::Button("Dang nhap", ImVec2(120, 0))) {
            if (kiemTraDangNhap(db, std::string(bufUser), std::string(bufPass))) {
                // Xoá buffer cho lần sau nếu cần
                // lưu tên user và xác định role (nếu trùng maSV thì là sinh viên)
                g_currentUser = std::string(bufUser);
                // Kiểm tra nếu có sinh viên tương ứng trong DB
                {
                    auto ds = docTatCaSinhVien(db);
                    bool found = false;
                    for (const auto& s : ds) if (s.maSV == g_currentUser) { found = true; break; }
                    g_isTeacher = !found;
                }
                bufUser[0] = '\0'; bufPass[0] = '\0'; thongBao.clear();
                ImGui::End();
                ImGui::Render();
                int display_w, display_h; glfwGetFramebufferSize(window, &display_w, &display_h);
                glViewport(0, 0, display_w, display_h);
                glClearColor(0.12f, 0.12f, 0.14f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
                glfwSwapBuffers(window);
                return true;
            } else {
                thongBao = "Dang nhap that bai: ten dang nhap hoac mat khau khong dung";
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Thoat", ImVec2(120, 0))) {
            ImGui::End();
            ImGui::Render();
            int display_w, display_h; glfwGetFramebufferSize(window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(0.12f, 0.12f, 0.14f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(window);
            return false;
        }

        if (!thongBao.empty()) {
            ImGui::Spacing();
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.3f, 1.0f), "%s", thongBao.c_str());
        }

        ImGui::End();

        // Render và swap
        ImGui::Render();
        int display_w, display_h; glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.12f, 0.12f, 0.14f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
}

// ============================================================
// TAB 1: THÊM SINH VIÊN MỚI (chỉ Mã SV + Họ tên, lưu ngay, chưa cần điểm)
// ============================================================
static void veTabThemSinhVien(DanhSachSinhVien& danhSach, CaySinhVien& caySV, sqlite3* db) {
    static char bufMaSV[32] = "";
    static char bufHoTen[128] = "";
    static std::string thongBao = "";

    ImGui::TextWrapped("Nhap Ma SV va Ho ten de tao 1 sinh vien moi. "
                        "Diem se duoc nhap sau o tab 'Quan ly sinh vien'.");
    ImGui::Spacing();

    ImGui::InputText("Ma sinh vien##sv", bufMaSV, IM_ARRAYSIZE(bufMaSV));
    ImGui::InputText("Ho ten##sv", bufHoTen, IM_ARRAYSIZE(bufHoTen));

    if (ImGui::Button("Them sinh vien", ImVec2(150, 30))) {
        if (strlen(bufMaSV) == 0 || strlen(bufHoTen) == 0) {
            thongBao = "Loi: chua nhap Ma SV hoac Ho ten";
        } else if (danhSach.timSinhVien(bufMaSV) != nullptr || caySV.timSinhVien(bufMaSV) != nullptr) {
            thongBao = "Loi: Ma SV nay da ton tai";
        } else {
            SinhVien sv;
            sv.maSV = bufMaSV;
            sv.hoTen = bufHoTen;

            danhSach.themSinhVien(sv);
            caySV.themSinhVien(sv);
            luuSinhVien(db, sv); // đồng bộ xuống SQLite ngay

            thongBao = "Da them sinh vien " + sv.maSV + ". Sang tab 'Quan ly sinh vien' de nhap diem.";

            bufMaSV[0] = '\0';
            bufHoTen[0] = '\0';
            // lưu mã SV vừa tạo để dễ tạo tài khoản
            static std::string lastAddedMaSV = "";
            lastAddedMaSV = sv.maSV;
        }
    }

    if (!thongBao.empty()) {
        ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "%s", thongBao.c_str());
    }

    // Nút tạo tài khoản SV cho mã vừa thêm
    {
        ImGui::Spacing();
        ImGui::TextDisabled("Tao tai khoan cho sinh vien (user = maSV, password mac dinh = 123456)");
        if (ImGui::Button("Tao tai khoan cho maSV gan nhat", ImVec2(240, 0))) {
            extern bool taoTaiKhoan(sqlite3* db, const std::string& tenDangNhap, const std::string& matKhau);
            // lấy lastAddedMaSV (static trong hàm) bằng cách reusing input
            // (không có cách trực tiếp vì hàm nội bộ). Thay vào đó, nếu bufMaSV rỗng,
            // tìm sinh viên cuối cùng trong DB
            auto ds = docTatCaSinhVien(db);
            if (!ds.empty()) {
                std::string last = ds.back().maSV;
                if (taoTaiKhoan(db, last, "123456")) {
                    ImGui::OpenPopup("ThongBaoTaoTaiKhoan");
                } else {
                    ImGui::OpenPopup("ThongBaoTaoTaiKhoan");
                }
            }
        }

        if (ImGui::BeginPopupModal("ThongBaoTaoTaiKhoan", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Hoan tat thao tac. (mat khau mac dinh: 123456)");
            if (ImGui::Button("OK")) ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }
    }
}

// ============================================================
// TAB 2: THÊM MÔN HỌC (quản lý danh mục môn học chung dùng chung
// cho toàn bộ sinh viên, hiển thị dạng dropdown ở tab Quản lý sinh viên)
// ============================================================
static void veTabThemMonHoc(sqlite3* db, std::vector<std::string>& danhSachMonChuan) {
    static char bufTenMonMoi[64] = "";
    static std::string thongBao = "";

    ImGui::TextWrapped("Danh muc mon hoc nay dung chung cho ca lop, "
                        "se hien ra de chon khi nhap diem cho tung sinh vien.");
    ImGui::Spacing();

    ImGui::InputText("Ten mon hoc moi", bufTenMonMoi, IM_ARRAYSIZE(bufTenMonMoi));

    if (ImGui::Button("Them mon hoc", ImVec2(150, 30))) {
        std::string ten = bufTenMonMoi;
        if (ten.empty()) {
            thongBao = "Loi: chua nhap ten mon hoc";
        } else if (std::find(danhSachMonChuan.begin(), danhSachMonChuan.end(), ten) != danhSachMonChuan.end()) {
            thongBao = "Loi: mon hoc nay da co trong danh muc";
        } else {
            danhSachMonChuan.push_back(ten);
            luuMonHocChuan(db, ten);
            thongBao = "Da them mon: " + ten;
            bufTenMonMoi[0] = '\0';
        }
    }

    if (!thongBao.empty()) {
        ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "%s", thongBao.c_str());
    }

    ImGui::Separator();
    ImGui::Text("Danh muc mon hoc hien co (%d):", (int)danhSachMonChuan.size());

    int indexCanXoa = -1;
    for (int i = 0; i < (int)danhSachMonChuan.size(); i++) {
        ImGui::PushID(i);
        ImGui::BulletText("%s", danhSachMonChuan[i].c_str());
        ImGui::SameLine();
        if (ImGui::SmallButton("Xoa")) {
            indexCanXoa = i;
        }
        ImGui::PopID();
    }

    // Xóa sau vòng lặp để tránh sửa vector đang được duyệt dở
    if (indexCanXoa >= 0) {
        xoaMonHocChuan(db, danhSachMonChuan[indexCanXoa]);
        danhSachMonChuan.erase(danhSachMonChuan.begin() + indexCanXoa);
    }

    if (danhSachMonChuan.empty()) {
        ImGui::TextDisabled("Chua co mon hoc nao. Them it nhat 1 mon o tren "
                             "truoc khi sang tab 'Quan ly sinh vien'.");
    }
}

// ============================================================
// TAB 3: QUẢN LÝ SINH VIÊN (chọn SV -> chọn môn -> nhập điểm ->
// xem kết quả ngay -> có thể xóa sinh viên tại đây)
// ============================================================
static void veTabQuanLySinhVien(DanhSachSinhVien& danhSach, CaySinhVien& caySV, sqlite3* db,
                                 const std::vector<std::string>& danhSachMonChuan) {
    static std::string maSVdangChon = "";
    static int monDangChonIndex = -1;
    static float diemGiuaKy = 0.0f;
    static float diemCuoiKy = 0.0f;
    static std::string thongBao = "";

    std::vector<SinhVien> ds = danhSach.layDanhSach();

    if (ds.empty()) {
        ImGui::TextDisabled("Chua co sinh vien nao. Sang tab 'Them sinh vien' de tao truoc.");
        return;
    }

    // ---- Combo chọn sinh viên (dùng maSV làm khóa, không dùng index,
    // để không bị lệch khi danh sách được sắp xếp lại) ----
    std::string nhanChonSV = "-- Chon sinh vien --";
    for (const SinhVien& sv : ds) {
        if (sv.maSV == maSVdangChon) {
            nhanChonSV = sv.maSV + " - " + sv.hoTen;
            break;
        }
    }

    if (ImGui::BeginCombo("Chon sinh vien", nhanChonSV.c_str())) {
        for (const SinhVien& sv : ds) {
            bool dangChon = (sv.maSV == maSVdangChon);
            std::string dong = sv.maSV + " - " + sv.hoTen;
            if (ImGui::Selectable(dong.c_str(), dangChon)) {
                maSVdangChon = sv.maSV;
                thongBao = "";
            }
        }
        ImGui::EndCombo();
    }

    if (maSVdangChon.empty()) {
        ImGui::TextDisabled("Chon 1 sinh vien o tren de bat dau quan ly diem.");
        return;
    }

    SinhVien* sv = caySV.timSinhVien(maSVdangChon);
    if (sv == nullptr) {
        // Sinh viên vừa bị xóa (có thể từ tab khác) -> reset lựa chọn
        maSVdangChon = "";
        return;
    }

    ImGui::Separator();
    ImGui::Text("Dang quan ly: %s - %s", sv->maSV.c_str(), sv->hoTen.c_str());

    if (ImGui::Button("Xoa sinh vien nay", ImVec2(140, 30))) {
        danhSach.xoaSinhVien(sv->maSV);
        caySV.xoaSinhVien(sv->maSV);
        xoaSinhVienDB(db, sv->maSV);
        thongBao = "Da xoa sinh vien " + maSVdangChon;
        maSVdangChon = ""; // reset lựa chọn vì sinh viên không còn tồn tại
        ImGui::Separator();
        if (!thongBao.empty()) {
            ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.3f, 1.0f), "%s", thongBao.c_str());
        }
        return; // dừng vẽ phần còn lại của tab vì sv đã bị xóa
    }

    ImGui::SameLine();
    if (ImGui::Button("Sua thong tin", ImVec2(140,30))) {
        ImGui::OpenPopup("SuaThongTinSV");
    }

    if (ImGui::BeginPopupModal("SuaThongTinSV", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        static char editHoTen[128];
        strncpy(editHoTen, sv->hoTen.c_str(), sizeof(editHoTen)); editHoTen[sizeof(editHoTen)-1]='\0';
        ImGui::InputText("Ho ten moi", editHoTen, IM_ARRAYSIZE(editHoTen));
        if (ImGui::Button("Luu")) {
            sv->hoTen = std::string(editHoTen);
            capNhatSinhVienDB(db, *sv);
            caySV.capNhatSinhVien(sv->maSV, *sv);
            danhSach.suaSinhVien(sv->maSV, *sv);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Huy")) { ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }

    ImGui::Separator();
    ImGui::Text("Nhap diem mon hoc:");

    if (danhSachMonChuan.empty()) {
        ImGui::TextDisabled("Chua co mon hoc nao trong danh muc. "
                             "Sang tab 'Them mon hoc' de them truoc.");
    } else {
        std::string nhanChonMon = "-- Chon mon hoc --";
        if (monDangChonIndex >= 0 && monDangChonIndex < (int)danhSachMonChuan.size()) {
            nhanChonMon = danhSachMonChuan[monDangChonIndex];
        }

        if (ImGui::BeginCombo("Chon mon hoc", nhanChonMon.c_str())) {
            for (int i = 0; i < (int)danhSachMonChuan.size(); i++) {
                bool dangChon = (monDangChonIndex == i);
                if (ImGui::Selectable(danhSachMonChuan[i].c_str(), dangChon)) {
                    monDangChonIndex = i;
                }
            }
            ImGui::EndCombo();
        }

        ImGui::InputFloat("Diem giua ky##ql", &diemGiuaKy, 0.5f);
        ImGui::InputFloat("Diem cuoi ky##ql", &diemCuoiKy, 0.5f);

        if (ImGui::Button("Luu diem mon nay", ImVec2(180, 30))) {
            if (monDangChonIndex < 0) {
                thongBao = "Loi: chua chon mon hoc";
            } else {
                std::string tenMon = danhSachMonChuan[monDangChonIndex];

                // Nếu môn này đã có điểm trước đó -> cập nhật đè lên,
                // chưa có -> thêm mới. Tránh 1 sinh viên bị trùng môn.
                MonHoc* monDaCo = nullptr;
                for (MonHoc& mon : sv->danhSachMon) {
                    if (mon.tenMon == tenMon) {
                        monDaCo = &mon;
                        break;
                    }
                }

                if (monDaCo != nullptr) {
                    monDaCo->diemGiuaKy = diemGiuaKy;
                    monDaCo->diemCuoiKy = diemCuoiKy;
                } else {
                    MonHoc monMoi;
                    monMoi.tenMon = tenMon;
                    monMoi.diemGiuaKy = diemGiuaKy;
                    monMoi.diemCuoiKy = diemCuoiKy;
                    // lấy hệ số môn chuẩn từ DB
                    auto hs = layHeSoMonHoc(db, tenMon);
                    monMoi.weightGK = hs.first;
                    monMoi.weightCK = hs.second;
                    monMoi.diemTongKet = tinhDiemTongKet(monMoi);
                    sv->danhSachMon.push_back(monMoi);
                }

                capNhatKetQua(*sv);
                capNhatSinhVienDB(db, *sv); // đồng bộ xuống SQLite
                caySV.capNhatSinhVien(sv->maSV, *sv);
                // Đồng bộ sang linked list để GUI và các thống kê dùng chung cập nhật đúng
                danhSach.suaSinhVien(sv->maSV, *sv);

                thongBao = "Da luu diem mon " + tenMon;
                diemGiuaKy = 0.0f;
                diemCuoiKy = 0.0f;
                monDangChonIndex = -1;
            }
        }
    }

    if (!thongBao.empty()) {
        ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "%s", thongBao.c_str());
    }

    ImGui::Separator();

    // ---- Hiển thị kết quả tổng thể ngay lập tức ----
    if (sv->danhSachMon.empty()) {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.3f, 1.0f), "Chua co diem mon nao");
    } else {
        ImGui::Text("Diem trung binh: %.2f", sv->diemTB);
        ImGui::SameLine();
        if (sv->datMonHoc) {
            ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), " -> DAU");
        } else {
            ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), " -> ROT");
        }

        ImGui::Text("Cac mon da nhap:");
        for (int i = 0; i < (int)sv->danhSachMon.size(); ++i) {
            MonHoc& mon = sv->danhSachMon[i];
            bool dat = monHocDat(mon);
            ImGui::PushID(i);
            ImGui::BulletText("%s - GK: %.1f, CK: %.1f, TK: %.2f -",
                mon.tenMon.c_str(), mon.diemGiuaKy, mon.diemCuoiKy, mon.diemTongKet);
            ImGui::SameLine();
            if (dat) {
                ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "Dau");
            } else {
                ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Rot");
            }
            ImGui::SameLine();
            if (ImGui::SmallButton("Sua")) {
                ImGui::OpenPopup("SuaDiemMon");
            }

            if (ImGui::BeginPopupModal("SuaDiemMon", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                static float editGK = 0.0f;
                static float editCK = 0.0f;
                editGK = mon.diemGiuaKy; editCK = mon.diemCuoiKy;
                ImGui::InputFloat("Diem giua ky", &editGK, 0.5f);
                ImGui::InputFloat("Diem cuoi ky", &editCK, 0.5f);
                if (ImGui::Button("Luu")) {
                    mon.diemGiuaKy = editGK;
                    mon.diemCuoiKy = editCK;
                    capNhatKetQua(*sv);
                    capNhatSinhVienDB(db, *sv);
                    caySV.capNhatSinhVien(sv->maSV, *sv);
                    danhSach.suaSinhVien(sv->maSV, *sv);
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine(); if (ImGui::Button("Huy")) ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
            }
            ImGui::PopID();
        }
    }
}

// ============================================================
// TAB 4: DANH SÁCH SINH VIÊN (bảng tổng quan, click xem chi tiết
// số môn đậu/rớt; xóa ở đây cũng đồng bộ vì dùng chung danhSach)
// ============================================================
static void veTabDanhSachSinhVien(DanhSachSinhVien& danhSach, CaySinhVien& caySV, sqlite3* db) {
    static char bufTimKiem[32] = "";
    static std::string maSVdangXem = "";

    if (ImGui::Button("Sap xep theo diem TB (giam dan)")) {
        danhSach.sapXepTheoDiemTB();
    }
    ImGui::SameLine();
    ImGui::SetNextItemWidth(200);
    ImGui::InputText("Tim theo Ma SV", bufTimKiem, IM_ARRAYSIZE(bufTimKiem));

    ImGui::Text("Tong so: %d | Dau: %d | Rot: %d | Chua co diem: %d | Diem TB toan lop: %.2f",
        danhSach.demSoLuong(), danhSach.demSoDau(), danhSach.demSoRot(),
        danhSach.demSoChuaCoDiem(), danhSach.diemTBToanLop());

    ImGui::Separator();

    std::vector<SinhVien> ds = danhSach.layDanhSach();
    std::string tuKhoa = bufTimKiem;

    if (ImGui::BeginTable("BangDanhSach", 7,
            ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY,
            ImVec2(0, 220))) {

        ImGui::TableSetupColumn("Ma SV");
        ImGui::TableSetupColumn("Ho ten");
        ImGui::TableSetupColumn("So mon");
        ImGui::TableSetupColumn("Diem TB");
        ImGui::TableSetupColumn("Ket qua chung");
        ImGui::TableSetupColumn("Xem");
        ImGui::TableSetupColumn("Xoa");
        ImGui::TableHeadersRow();

        for (const SinhVien& sv : ds) {
            if (!tuKhoa.empty() && sv.maSV.find(tuKhoa) == std::string::npos) {
                continue;
            }

            ImGui::PushID(sv.maSV.c_str());
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
            if (ImGui::SmallButton("Xem chi tiet")) {
                maSVdangXem = (maSVdangXem == sv.maSV) ? "" : sv.maSV; // bấm lại để đóng
            }

            ImGui::TableNextColumn();
            if (ImGui::SmallButton("Xoa")) {
                danhSach.xoaSinhVien(sv.maSV);
                caySV.xoaSinhVien(sv.maSV);
                xoaSinhVienDB(db, sv.maSV);
                if (maSVdangXem == sv.maSV) maSVdangXem = "";
            }

            ImGui::PopID();
        }

        ImGui::EndTable();
    }

    // ---- Khu vực chi tiết: số môn đậu/rớt của sinh viên đang xem ----
    if (!maSVdangXem.empty()) {
        SinhVien* svXem = caySV.timSinhVien(maSVdangXem);
        if (svXem == nullptr) {
            maSVdangXem = ""; // đã bị xóa
        } else {
            ImGui::Separator();
            ImGui::Text("Chi tiet: %s - %s", svXem->maSV.c_str(), svXem->hoTen.c_str());

            if (svXem->danhSachMon.empty()) {
                ImGui::TextDisabled("Sinh vien nay chua co diem mon nao.");
            } else {
                ImGui::Text("So mon dau: %d | So mon rot: %d",
                    demSoMonDat(*svXem), demSoMonRot(*svXem));

                for (const MonHoc& mon : svXem->danhSachMon) {
                    bool dat = monHocDat(mon);
                    ImGui::BulletText("%s - Diem tong ket: %.2f -",
                        mon.tenMon.c_str(), mon.diemTongKet);
                    ImGui::SameLine();
                    if (dat) {
                        ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "Dau");
                    } else {
                        ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Rot");
                    }
                }
            }
        }
    }
}

// ============================================================
// Vẽ 1 khung hình giao diện — được gọi liên tục trong vòng lặp chính
// Trả về false khi người dùng đóng cửa sổ
// ============================================================
bool veKhungHinh(DanhSachSinhVien& danhSach, CaySinhVien& caySV, sqlite3* db,
                  std::vector<std::string>& danhSachMonChuan) {
    if (glfwWindowShouldClose(window)) {
        return false;
    }

    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowSize(ImVec2(1000, 720), ImGuiCond_FirstUseEver);
    ImGui::Begin("Quan ly diem sinh vien");

    // Nếu user hiện tại là sinh viên, hiển thị chế độ xem điểm đơn giản
    if (!g_isTeacher) {
        SinhVien* my = caySV.timSinhVien(g_currentUser);
        if (my == nullptr) {
            ImGui::TextDisabled("Khong tim thay thong tin sinh vien. Co the tai khoan chua duoc tao hoac maSV khong dung.");
        } else {
            ImGui::Text("Sinh vien: %s - %s", my->maSV.c_str(), my->hoTen.c_str());
            ImGui::Separator();
            if (my->danhSachMon.empty()) {
                ImGui::TextDisabled("Chua co diem mon nao.");
            } else {
                ImGui::Text("Diem trung binh: %.2f", my->diemTB);
                ImGui::Text("Cac mon:");
                for (const MonHoc& mon : my->danhSachMon) {
                    ImGui::BulletText("%s - GK: %.1f, CK: %.1f, TK: %.2f", mon.tenMon.c_str(), mon.diemGiuaKy, mon.diemCuoiKy, mon.diemTongKet);
                }
            }
        }
        ImGui::Separator();
        if (ImGui::Button("Thoat")) {
            // Đóng cửa sổ
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
        ImGui::End();
        // Render và swap sau hàm gọi chính
        ImGui::Render();
        int display_w, display_h; glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.12f, 0.12f, 0.14f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        return true;
    }

    if (ImGui::BeginTabBar("TabChinh")) {

        if (ImGui::BeginTabItem("1. Them sinh vien")) {
            veTabThemSinhVien(danhSach, caySV, db);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("2. Them mon hoc")) {
            veTabThemMonHoc(db, danhSachMonChuan);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("3. Quan ly sinh vien")) {
            veTabQuanLySinhVien(danhSach, caySV, db, danhSachMonChuan);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("4. Danh sach sinh vien")) {
            veTabDanhSachSinhVien(danhSach, caySV, db);
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
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