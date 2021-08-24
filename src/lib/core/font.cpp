#include "font.h"

#include <fstream>

#include "system.h"


namespace
{

class FaceDeleter {
public:
    FaceDeleter(FT_Face &face)
        : face_(face) {}

    ~FaceDeleter() {
        if (face_) {
            FT_Done_Face(face_);
            face_ = nullptr;
        }
    }

private:
    FT_Face &face_;
};

}

namespace font
{

static void readFileToMemory(const fs::path &path, std::basic_string<unsigned char> &to)
{
    std::ifstream ifs(path, std::ios::binary);
    to.resize(fs::file_size(path));
    ifs.read(reinterpret_cast<char *>(to.data()), to.size());
}

FontFile::FontFile(const FontContext &context, const fs::path &path, bool loadToMemory)
    : context_(&context), path_(fs::absolute(path))
{
    isValid_ = false;

    FT_Face ftFace = nullptr;

    FaceDeleter faceDeleter(ftFace);

    if (loadToMemory) {
        readFileToMemory(path, data_);
        const FT_Error error = FT_New_Memory_Face(context.ftLib_, data_.data(), static_cast<FT_Long>(data_.size()), -1, &ftFace);
        if (error != 0) {
            ftFace = nullptr;
            return;
        }
        
    } else {
        const FT_Error error = FT_New_Face(context.ftLib_, path_.generic_string().c_str(), -1, &ftFace);
        if (error != 0) {
            ftFace = nullptr;
            return;
        }
    }

    if (ftFace) {
        faceCount_ = ftFace->num_faces;
    }

    isValid_ = true;
}

FontFace::FontFace(const FontFile &file, long faceIndex)
    : file_(&file)
{
    if (!file.data_.empty()) {
        const FT_Error error = FT_New_Memory_Face(file.context_->ftLib_, file.data_.data(), static_cast<FT_Long>(file.data_.size()), faceIndex, &ftFace_);
        if (error != 0) {
            ftFace_ = nullptr;
        }
    } else {
        const FT_Error error = FT_New_Face(file.context_->ftLib_, file.path_.generic_string().c_str(), faceIndex, &ftFace_);
        if (error != 0) {
            ftFace_ = nullptr;
        }
    }
}

void FontFace::setPointSize(int pt)
{
    thread_local int hDpi = SystemUtil::screenHorizontalDpi();
    thread_local int vDpi = SystemUtil::screenVerticalDpi();
    const FT_Error error = FT_Set_Char_Size(ftFace_, 0, pt * 64, hDpi, vDpi);
    if (error != 0) {
        std::ostringstream ss;
        ss << "FontFace::setPointSize() failed";
        throw std::logic_error(ss.str());
    }
}

int64_t FontFace::mapUnicodeToGlyphIndex(char32_t unicode) const
{
    return FT_Get_Char_Index(ftFace_, unicode);
}

void FontFace::loadGlyph(int64_t glyphIndex)
{
    const FT_Error error = FT_Load_Glyph(ftFace_, static_cast<FT_UInt>(glyphIndex), FT_LOAD_DEFAULT);
    if (error != 0) {
        std::ostringstream ss;
        ss << "FontFace::loadGlyph() failed with glyphIndex [" << glyphIndex << "]";
        if (error == FT_Err_Invalid_Size_Handle) {
            ss << " because FontFace::setPointSize() not called";
        }
        throw std::logic_error(ss.str());
    }
}

void FontFace::renderGlyph()
{
    const FT_Error error = FT_Render_Glyph(ftFace_->glyph, FT_RENDER_MODE_NORMAL);
    if (error != 0) {
        std::ostringstream ss;
        ss << "FontFace::renderGlyph() failed";
        throw std::logic_error(ss.str());
    }
}

}
