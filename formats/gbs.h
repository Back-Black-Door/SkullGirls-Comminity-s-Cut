#pragma once
#include <stdint.h>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>
#include "reader_writer.h"

namespace fs = std::filesystem;
namespace gbs {
    enum config : uint32_t {
        None = 0,
        add_new_fonts = 1 << 0,
        divide_coords = 1 << 1,
        calculate_texture_id = 1 << 2,
        All = add_new_fonts | divide_coords | calculate_texture_id
    };

    constexpr config operator|(config a, config b) noexcept {
        return static_cast<config>(
            static_cast<uint32_t>(a) |
            static_cast<uint32_t>(b));
    }

    constexpr bool has_flag(config value, config flag) noexcept {
        return (static_cast<uint32_t>(value) &
            static_cast<uint32_t>(flag)) != 0;
    }

class gbs_t {
    public:
        class font_t;
        class char_t;
        class texture_t;
        gbs_t(const fs::path pathtoread);
        void write(fs::path const pathtowrite);
    private:
        void _read();
    public:
        class font_t {
        public:
            font_t(std::vector<unsigned char>& buffer, size_t ptr);
        private:
            void _read(std::vector<unsigned char>& buffer, size_t ptr);
        private:
            std::string m_gfnt_lable;
            uint32_t m_font_lenght;
            uint32_t m_font_id;
            std::string m_font_name;
            uint32_t m_font_size;
            uint32_t m_atlas_w;
            uint32_t m_atlas_h;
            uint32_t m_max_top;
            uint32_t m_atlas_count;
            uint32_t m_chars_count;
            std::vector<char_t> m_chars;
            friend gbs_t merge(gbs_t& first_gbs, gbs_t& second_gbs, config config);
        public:
            std::string gfnt_lable() const { return m_gfnt_lable; }
            uint32_t font_lenght() const { return m_font_lenght; }
            uint32_t font_id() const { return m_font_id; }
            std::string font_name() const { return m_font_name; }
            uint32_t font_size() const { return m_font_size; }
            uint32_t atlas_w() const { return m_atlas_w; }
            uint32_t atlas_h() const { return m_atlas_h; }
            uint32_t max_top() const { return m_max_top; }
            uint32_t atlas_count() const { return m_atlas_count; }
            uint32_t chars_count() const { return m_chars_count; }
            std::vector<char_t> chars() const { return m_chars; }
        public:
            size_t size() const { return m_font_lenght; }
        };
        class char_t {
        public:
            char_t(std::vector <unsigned char>& file_buffer, size_t ptr_f);
            ~char_t() = default;
        private:
            void _read(std::vector <unsigned char>& file_buffer, size_t ptr_f);
        private:
            std::string m_char_code;
            uint32_t m_is_image_glyph;
            uint32_t m_char_x_offset;
            uint32_t m_char_y_offset;
            uint32_t m_char_w;
            uint32_t m_char_h;
            uint32_t m_char_top;
            uint32_t m_char_advance;
            uint32_t m_char_left_bearning;
            uint32_t m_char_atlas_index;
            friend gbs_t merge(gbs_t& first_gbs, gbs_t& second_gbs, config config);
        public:
            std::string char_code() const { return m_char_code; }
            uint32_t is_image_glyph() const { return m_is_image_glyph; }
            uint32_t char_x_offset() const { return m_char_x_offset; }
            uint32_t char_y_offset() const { return m_char_y_offset; }
            uint32_t char_w() const { return m_char_w; }
            uint32_t char_h() const { return m_char_h; }
            uint32_t char_top() const { return m_char_top; }
            uint32_t char_advance() const { return m_char_advance; }
            uint32_t char_left_bearning() const { return m_char_left_bearning; }
            uint32_t char_atlas_index() const { return m_char_atlas_index; }
        };
        class texture_t {
        public:
            texture_t(std::vector<unsigned char>& buffer, size_t ptr);
        private:
            void _read(std::vector<unsigned char>& buffer, size_t ptr);
        private:
            uint16_t m_id;
            uint16_t m_type;
            std::string m_path;
            friend gbs_t merge(gbs_t& first_gbs, gbs_t& second_gbs, config config);
        public:
            uint16_t id() const { return m_id; }
            uint16_t type() const { return m_type; }
            std::string path() const { return m_path; }
        public:
            size_t size() const { return 0x11C; }
        };
    private:
        std::string m_gbsc_header = "CSGG";
        uint32_t m_file_size;
        std::string m_data_version;
        uint32_t m_scene_id;
        uint32_t m_fonts_count;
        uint32_t m_textures_count;
        uint32_t m_sounds_count;
        uint32_t m_views_count;
        uint32_t m_messages_count;
        uint32_t m_fonts_offset;
        uint32_t m_textures_offset;
        uint32_t m_sounds_offset;
        uint32_t m_view_offset;
        uint32_t m_messages_offset;
        std::vector<font_t> m_fonts;
        std::vector<texture_t> m_textures;
        friend gbs_t merge(gbs_t& first_gbs, gbs_t& second_gbs, config config);
    public:
        std::string gbsc_header() const { return m_gbsc_header; }
        uint32_t file_size() const { return m_file_size; }
        std::string data_version() const { return m_data_version; }
        uint32_t scene_id() const { return m_scene_id; }
        uint32_t fonts_count() const { return m_fonts_count; }
        uint32_t textures_count() const { return m_textures_count; }
        uint32_t sounds_count() const { return m_sounds_count; }
        uint32_t views_count() const { return m_views_count; }
        uint32_t messages_count() const { return m_messages_count; }
        uint32_t fonts_offset() const { return m_fonts_offset; }
        uint32_t textures_offset() const { return m_textures_offset; }
        uint32_t sounds_offset() const { return m_sounds_offset; }
        uint32_t view_offset() const { return m_view_offset; }
        uint32_t messages_offset() const { return m_messages_offset; }
        std::vector<font_t> fonts() const { return m_fonts; }
        std::vector<texture_t> textures() const { return m_textures; }
    private:
        std::vector<unsigned char> file_buffer;
    public:
        //  std::vector<unsigned char> file_buffer() const { return file_buffer; }
    };
    gbs_t merge(gbs_t& first_gbs, gbs_t& second_gbs, config config = config::None);
}

