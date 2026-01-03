#pragma once
#include "axmol/axmol.h"
#include <map>
#include <string>

class ColorPalette {
public:
    // Bảng màu Flow Free chuẩn (sặc sỡ, dễ phân biệt)
    static ax::Color32 getColor(const std::string& colorId)
    {
        static std::map<std::string, ax::Color32> colors = {
            // Màu cơ bản
            {"A", ax::Color32(255, 59, 48, 255)},    // Đỏ tươi
            {"B", ax::Color32(52, 199, 89, 255)},    // Xanh lá
            {"C", ax::Color32(0, 122, 255, 255)},    // Xanh dương
            {"D", ax::Color32(255, 204, 0, 255)},    // Vàng
            {"E", ax::Color32(255, 149, 0, 255)},    // Cam
            {"F", ax::Color32(175, 82, 222, 255)},   // Tím
            {"G", ax::Color32(255, 45, 85, 255)},    // Hồng
            {"H", ax::Color32(88, 86, 214, 255)},    // Xanh indigo
            
            // Màu mở rộng (cho level khó)
            {"I", ax::Color32(90, 200, 250, 255)},   // Xanh cyan
            {"J", ax::Color32(255, 55, 95, 255)},    // Đỏ hồng
            {"K", ax::Color32(50, 215, 75, 255)},    // Xanh mint
            {"L", ax::Color32(191, 90, 242, 255)},   // Tím nhạt
        };
        
        auto it = colors.find(colorId);
        if (it != colors.end())
            return it->second;
        
        // Màu mặc định nếu không tìm thấy
        return ax::Color32(128, 128, 128, 255);
    }
};