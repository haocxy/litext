#include "doc_cutter_by_row.h"


namespace doc
{


DocCutterByRow::DocCutterByRow(const fs::path &filePath, Charset charset, uintmax_t partSizeHint)
	: filePath_(filePath)
	, charset_(charset)
	, partSizeHint_(partSizeHint)
	, fileSize_(fs::file_size(filePath))
	, ifs_(filePath, std::ios::binary)
{

}

static void skipRowEndForAscii(std::ifstream &ifs) {
	constexpr char eof = std::ifstream::traits_type::eof();
	char ch = 0;
	while ((ch = ifs.get()) != eof) {
		if (ch == '\r') {
			char next = 0;
			if ((next = ifs.get()) != eof) {
				if (next == '\n') {
					return;
				} else {
					ifs.unget();
					continue;
				}
			} else {
				return;
			}
		} else if (ch == '\n') {
			return;
		} else {
			continue;
		}
	}
}

static void skipRowEndForUnknown(std::ifstream &ifs) {
	skipRowEndForAscii(ifs);
}

// 把文件流ifs定位到换行字节组之后的下一个字节
void skipRowEnd(std::ifstream &ifs, Charset charset) {
	switch (charset) {
	case Charset::Ascii:
	case Charset::UTF_8:
	case Charset::GB18030:
		skipRowEndForAscii(ifs);
		return;
	default:
		skipRowEndForUnknown(ifs);
		return;
	}
}

std::optional<OffsetRange> DocCutterByRow::next()
{
	if (!ifs_) {
		return std::nullopt;
	}

	const uintmax_t offset = ifs_.tellg();
	const uintmax_t remain = fileSize_ - offset;

	if (remain <= partSizeHint_) {
		ifs_.close();
		return OffsetRange(offset, remain);
	}

	ifs_.seekg(partSizeHint_, std::ios::cur);
	if (!ifs_) {
		ifs_.close();
		return OffsetRange(offset, remain);
	}

	skipRowEnd(ifs_, charset_);

	if (ifs_) {
		const uintmax_t newOffset = ifs_.tellg();
		const uintmax_t delta = newOffset - offset;
		return OffsetRange(offset, delta);
	} else {
		ifs_.close();
		return OffsetRange(offset, remain);
	}
}


}
