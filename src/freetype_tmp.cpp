#include <string>

#include <third/freetype_wrapper/freetype/include/ft2build.h>
#include <third/freetype_wrapper/freetype/include/freetype/freetype.h>

#include "core/logger.h"


void testFreeType(const std::string &file, int dpix, int dpiy)
{
    LOGD << "testFreeType() for file [" << file << "]";

    FT_Library lib = nullptr;
    FT_Error error = 0;

    error = FT_Init_FreeType(&lib);

    FT_Face face = nullptr;
    error = FT_New_Face(lib, file.c_str(), 0, &face);
    if (error != 0) {
        LOGE << "error on FT_New_Face(): " << error;
    }

    error = FT_Set_Char_Size(face, 0, 14 * 64, dpix, dpiy);

    FT_UInt index = FT_Get_Char_Index(face, 'X');

    error = FT_Load_Glyph(face, index, FT_LOAD_DEFAULT);
    LOGI << "testFreeType() family [" << face->family_name << "] style [" << face->style_name << "]";


    FT_Done_Face(face);
    face = nullptr;

    error = FT_Done_FreeType(lib);
    lib = nullptr;
}
