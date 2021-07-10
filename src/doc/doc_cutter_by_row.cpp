#include "doc_cutter_by_row.h"


namespace doc
{


DocCutterByRow::DocCutterByRow(const fs::path &filePath, const std::string &charset, uintmax_t partSizeHint)
	: filePath_(filePath)
	, charset_(charset)
	, partSizeHint_(partSizeHint)
	, bytecount_(fs::file_size(filePath))
	, ifs_(filePath, std::ios::binary)
{

}

RowRange DocCutterByRow::next()
{
	if (!hasNext()) {
		return RowRange();
	}


	return RowRange(); // TODO
}


}
