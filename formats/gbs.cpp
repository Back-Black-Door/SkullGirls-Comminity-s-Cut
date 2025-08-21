#include "gbs.h"

namespace fs = std::filesystem;
using namespace reader;
using namespace writer;


namespace gbs {
gbs_t::gbs_t(const fs::path pathtoread) {
    try {
        std::fstream File;
        File.open(pathtoread, std::ios::out | std::ios::in | std::ios::binary | std::ios::ate);
        if (File.is_open()) {
            auto size = File.tellg();
            File.seekg(0);  // Возвращаемся в начало для чтения
            file_buffer.resize(size);
            File.read(reinterpret_cast<char*>(file_buffer.data()), size);
            _read();
        }
        else throw;
    }
    catch (...) {
        //_clean_up();
        throw;
    }
}

void gbs_t::_read() {
    m_gbsc_header = readBuffer_VectorUnChar_to_String(file_buffer, 0, 4);
    m_file_size = LE_readBuffer_VectorUnChar_to_UnInt32(file_buffer, 4);
    m_data_version = readBuffer_VectorUnChar_to_String(file_buffer, 8, 4);
    m_scene_id = LE_readBuffer_VectorUnChar_to_UnInt32(file_buffer, 12);
    m_fonts_count = LE_readBuffer_VectorUnChar_to_UnInt32(file_buffer, 16);
    m_textures_count = LE_readBuffer_VectorUnChar_to_UnInt32(file_buffer, 20);
    m_sounds_count = LE_readBuffer_VectorUnChar_to_UnInt32(file_buffer, 24);
    m_views_count = LE_readBuffer_VectorUnChar_to_UnInt32(file_buffer, 28);
    m_messages_count = LE_readBuffer_VectorUnChar_to_UnInt32(file_buffer, 32);
    m_fonts_offset = LE_readBuffer_VectorUnChar_to_UnInt32(file_buffer, 36);
    m_textures_offset = LE_readBuffer_VectorUnChar_to_UnInt32(file_buffer, 40);
    m_sounds_offset = LE_readBuffer_VectorUnChar_to_UnInt32(file_buffer, 44);
    m_view_offset = LE_readBuffer_VectorUnChar_to_UnInt32(file_buffer, 48);
    m_messages_offset = LE_readBuffer_VectorUnChar_to_UnInt32(file_buffer, 52);
    size_t ptr = 56 + m_fonts_offset;
    const int l_fonts = fonts_count();
    for (int i = 0; i < l_fonts; i++) {
        font_t current(file_buffer, ptr);
        m_fonts.push_back(current);
        ptr += current.size();
    }
    const int l_textures = textures_count();
    for (int i = 0; i < l_textures; i++) {
        texture_t current(file_buffer, ptr);
        m_textures.push_back(current);
        ptr += current.size();
    }
    file_buffer.erase(file_buffer.begin(), file_buffer.begin() + ptr);
}

gbs_t::font_t::font_t(std::vector<unsigned char>& buffer, size_t ptr) {
    try {
        _read(buffer, ptr);
    }
    catch (...) {
        throw;
    }
}

void gbs_t::font_t::_read(std::vector<unsigned char>& file_buffer, size_t ptr) {
    m_gfnt_lable = readBuffer_VectorUnChar_to_String(file_buffer, ptr, 4);
    m_font_lenght = LE_readBuffer_VectorUnChar_to_UnInt32(file_buffer, ptr + 4);
    m_font_id = LE_readBuffer_VectorUnChar_to_UnInt32(file_buffer, ptr + 8);
    m_font_name = readBuffer_VectorUnChar_to_String(file_buffer, ptr + 12, 64);
    m_font_size = LE_readBuffer_VectorUnChar_to_UnInt32(file_buffer, ptr + 76);
    m_atlas_w = LE_readBuffer_VectorUnChar_to_UnInt32(file_buffer, ptr + 80);
    m_atlas_h = LE_readBuffer_VectorUnChar_to_UnInt32(file_buffer, ptr + 84);
    m_max_top = LE_readBuffer_VectorUnChar_to_UnInt32(file_buffer, ptr + 88);
    m_atlas_count = LE_readBuffer_VectorUnChar_to_UnInt32(file_buffer, ptr + 92);
    m_chars_count = LE_readBuffer_VectorUnChar_to_UnInt32(file_buffer, ptr + 96);
    const int l_chars = m_chars_count;
    size_t ptr_f = ptr + 100;
    for (int i = 0; i < l_chars; i++) {
        m_chars.push_back(char_t(file_buffer, ptr_f));
        ptr_f += 0x28;
    }
}

gbs_t::char_t::char_t(std::vector <unsigned char>& file_buffer, size_t ptr_f) {

    try {
        _read(file_buffer, ptr_f);
    }
    catch (...) {
        throw;
    }
}

void gbs_t::char_t::_read(std::vector <unsigned char>& file_buffer, size_t ptr) {
    m_char_code = readBuffer_VectorUnChar_to_String(file_buffer, ptr, 4);
    m_is_image_glyph = LE_readBuffer_VectorUnChar_to_UnInt32(file_buffer, ptr + 4);
    m_char_x_offset = LE_readBuffer_VectorUnChar_to_UnInt32(file_buffer, ptr + 8);
    m_char_y_offset = LE_readBuffer_VectorUnChar_to_UnInt32(file_buffer, ptr + 12);
    m_char_w = LE_readBuffer_VectorUnChar_to_UnInt32(file_buffer, ptr + 16);
    m_char_h = LE_readBuffer_VectorUnChar_to_UnInt32(file_buffer, ptr + 20);
    m_char_top = LE_readBuffer_VectorUnChar_to_UnInt32(file_buffer, ptr + 24);
    m_char_advance = LE_readBuffer_VectorUnChar_to_UnInt32(file_buffer, ptr + 28);
    m_char_left_bearning = LE_readBuffer_VectorUnChar_to_UnInt32(file_buffer, ptr + 32);
    m_char_atlas_index = LE_readBuffer_VectorUnChar_to_UnInt32(file_buffer, ptr + 36);
}

gbs_t::texture_t::texture_t(std::vector<unsigned char>& buffer, size_t ptr) {
    try {
        _read(buffer, ptr);
    }
    catch (...) {
        throw;
    }
}

void gbs_t::texture_t::_read(std::vector<unsigned char>& file_buffer, size_t ptr) {
    m_id = (__int16)file_buffer[ptr] +
        ((__int16)file_buffer[ptr + 1] << 8);
    m_type = (__int16)file_buffer[ptr + 2] +
        ((__int16)file_buffer[ptr + 3] << 8);
    m_path = readBuffer_VectorUnChar_to_String(file_buffer, ptr + 4, 260);
}
 
void gbs_t::write(fs::path const pathtowrite) {
    std::ofstream export_file(pathtowrite.string(), std::ios::out | std::ios::binary);
    if (!export_file.is_open()) {
        throw;
    }
    std::vector<unsigned char> buffer;
    appendString(buffer, m_gbsc_header);
    appendLE32(buffer, m_file_size);
    appendString(buffer, m_data_version);
    appendLE32(buffer, uint32_t(m_scene_id));
    appendLE32(buffer, uint32_t(m_fonts_count));
    appendLE32(buffer, uint32_t(m_textures_count));
    appendLE32(buffer, uint32_t(m_sounds_count));
    appendLE32(buffer, uint32_t(m_views_count));
    appendLE32(buffer, uint32_t(m_messages_count));
    appendLE32(buffer, uint32_t(m_fonts_offset));
    appendLE32(buffer, uint32_t(m_textures_offset));
    appendLE32(buffer, uint32_t(m_sounds_offset));
    appendLE32(buffer, uint32_t(m_view_offset));
    appendLE32(buffer, uint32_t(m_messages_offset));
    for (const auto& font : m_fonts) {
        appendString(buffer, font.gfnt_lable());
        appendLE32(buffer, font.font_lenght());
        appendLE32(buffer, font.font_id());
        appendString(buffer, font.font_name());
        appendLE32(buffer, font.font_size());
        appendLE32(buffer, font.atlas_w());
        appendLE32(buffer, font.atlas_h());
        appendLE32(buffer, font.max_top());
        appendLE32(buffer, font.atlas_count());
        appendLE32(buffer, font.chars_count());
        for (const auto& letter : font.chars()) {
            appendString(buffer, letter.char_code());
            appendLE32(buffer, letter.is_image_glyph());
            appendLE32(buffer, letter.char_x_offset());
            appendLE32(buffer, letter.char_y_offset());
            appendLE32(buffer, letter.char_w());
            appendLE32(buffer, letter.char_h());
            appendLE32(buffer, letter.char_top());
            appendLE32(buffer, letter.char_advance());
            appendLE32(buffer, letter.char_left_bearning());
            appendLE32(buffer, letter.char_atlas_index());
        }
    }
    for (const auto& texture : m_textures) {
        appendLE16(buffer, texture.id());
        appendLE16(buffer, texture.type());
        appendString(buffer, texture.path());
        appendLE16(buffer, texture.id());
        appendLE16(buffer, texture.type());

        appendLE64(buffer, 0);

        appendLE16(buffer, 0);
        appendLE16(buffer, uint16_t(0x3f80));
        appendLE16(buffer, 0);
        appendLE16(buffer, uint16_t(0x3f80));
    }
    buffer.insert(buffer.end(), file_buffer.begin(), file_buffer.end());
    export_file.write(reinterpret_cast<char*>(buffer.data()), buffer.size());
}

gbs_t merge(gbs_t& first_gbs, gbs_t& second_gbs, config config) {
    gbs_t merged_gbs = first_gbs; // Начинаем с копии первого файла

    double divider = 1.5;
    double divider2 = 1.45;

    // Объединяем шрифты
    for (const auto& font : second_gbs.m_fonts) {
        bool font_exists = false;

        // Ищем шрифт с таким же ID в объединенном файле
        for (auto& existing_font : merged_gbs.m_fonts) {
            uint32_t old_atlas_count = existing_font.m_atlas_count;
            if (existing_font.m_font_id == font.m_font_id) {
                font_exists = true;

                // Если шрифт найден, проверяем его символы
                for (const auto& letter : font.m_chars) {
                    bool letter_exists = false;
                    if (existing_font.m_max_top < round(double(font.m_max_top) / divider) && has_flag(config, divide_coords)) {
                        existing_font.m_max_top = round(double(font.m_max_top) / divider);
                    }
                    // Проверяем, есть ли уже такой символ
                    for (const auto& existing_letter : existing_font.m_chars) {
                        if (existing_letter.m_char_code == letter.m_char_code) {
                            letter_exists = true;
                            break; // Символ уже существует, пропускаем
                        }
                    }

                    // Если символа нет, добавляем его
                    if (!letter_exists) {
                        gbs_t::char_t current = letter;
                        //768 --> 512;
                        if (has_flag(config, divide_coords)) {
                            current.m_char_x_offset = round(double(current.m_char_x_offset) / divider);
                            current.m_char_y_offset = round(double(current.m_char_y_offset) / divider);
                            current.m_char_w = round(double(current.m_char_w) / divider);
                            current.m_char_h = round(double(current.m_char_h) / divider);
                            current.m_char_top = round(double(current.m_char_top) / divider);
                            //current.m_char_left_bearning = round(double(current.m_char_left_bearning) / divider);
                            current.m_char_advance = round(double(current.m_char_advance) / divider2);
                        }
                        current.m_char_atlas_index += old_atlas_count;

                        existing_font.m_chars.push_back(current);
                        if (current.m_char_atlas_index + 1 > existing_font.m_atlas_count) {
                            existing_font.m_atlas_count = current.m_char_atlas_index + 1;
                        }
                        existing_font.m_chars_count++;
                        existing_font.m_font_lenght += 0x28; // Размер структуры char_t
                        
                        merged_gbs.m_file_size += 0x28;
                        merged_gbs.m_textures_offset += 0x28;
                        merged_gbs.m_sounds_offset += 0x28;
                        merged_gbs.m_view_offset += 0x28;
                        merged_gbs.m_messages_offset += 0x28;
                    }
                }

                break; // Шрифт обработан, переходим к следующему
            }
        }

        // Если шрифта с таким ID нет, добавляем его целиком (если add_new_fonts == TRUE)
        if (!font_exists && has_flag(config, add_new_fonts)) {
            merged_gbs.m_fonts.push_back(font);
            merged_gbs.m_fonts_count++;
        }
    }

    // Обьеденям тексутры
    for (const auto& texture : second_gbs.m_textures) {
        bool texture_exists = false;
        for (const auto& existing_texture : merged_gbs.m_textures) {
            if (texture.m_id == existing_texture.m_id) {
                texture_exists = true;
            }
            if (!texture_exists) {
                gbs_t::texture_t current = texture;
                if (has_flag(config, calculate_texture_id)) {
                    current.m_id = merged_gbs.m_textures.back().m_id + 1;
                }
                merged_gbs.m_textures.push_back(current);
                merged_gbs.m_textures_count++;

                merged_gbs.m_file_size += 0x11C;
                merged_gbs.m_sounds_offset += 0x11C;
                merged_gbs.m_view_offset += 0x11C;
                merged_gbs.m_messages_offset += 0x11C;
                break;
            }
        }
    }
    
    // ... (остальная логика слияния текстур и других данных someday, but not today!)
    return merged_gbs;
}

}