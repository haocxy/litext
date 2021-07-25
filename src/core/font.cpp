#include "font.h"

#include <fstream>

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

template <typename T, typename U>
static bool hasFlag(T bitset, U mask)
{
    return (bitset & mask) == mask;
}

FontFile::FontFile(FontContext &context, const fs::path &path, bool loadToMemory)
    : context_(context), path_(fs::absolute(path))
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
        isItalic_ = hasFlag(ftFace->style_flags, FT_STYLE_FLAG_ITALIC);
        isBold_ = hasFlag(ftFace->style_flags, FT_STYLE_FLAG_BOLD);
    }

    isValid_ = true;
}

FontFace::FontFace(const FontFile &file, long faceIndex)
    : file_(file)
{
    if (!file.data_.empty()) {
        const FT_Error error = FT_New_Memory_Face(file.context_.ftLib_, file.data_.data(), static_cast<FT_Long>(file.data_.size()), faceIndex, &ftFace_);
        if (error != 0) {
            ftFace_ = nullptr;
        }
    } else {
        const FT_Error error = FT_New_Face(file.context_.ftLib_, file.path_.generic_string().c_str(), faceIndex, &ftFace_);
        if (error != 0) {
            ftFace_ = nullptr;
        }
    }
}

}
