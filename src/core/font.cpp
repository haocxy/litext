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

FontFile::FontFile(FontContext &context, const fs::path &path, bool loadToMemory)
    : context_(context), path_(path)
{
    isValid_ = false;

    FT_Face ftFace = nullptr;

    FaceDeleter faceDeleter(ftFace);

    if (loadToMemory) {
        readFileToMemory(path, data_);
        const FT_Error error = FT_New_Memory_Face(context.ftLib_, data_.data(), static_cast<FT_Long>(data_.size()), -1, &ftFace);
        if (error != 0) {
            return;
        }
        
    } else {
        const FT_Error error = FT_New_Face(context.ftLib_, fs::absolute(path).generic_string().c_str(), -1, &ftFace);
        if (error != 0) {
            return;
        }
    }

    if (ftFace) {
        faceCount_ = ftFace->num_faces;
    }

    isValid_ = true;
}

}
