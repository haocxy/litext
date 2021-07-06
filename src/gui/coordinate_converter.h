#pragma once

#include "size.h"
#include "page.h"
#include "view_loc.h"


namespace gui
{

// �Ѳ�ͬ����ϵ�е�λ���໥ת��
// ����������ϵ���ĵ����ꡢ��ͼ���ꡢGUI��������
// �ⲿ���߼�����TextArea��������Ϊ���ƺ���̫����ʵ�ִ���ϳ������Ե�������
class CoordinateConverter {
public:
	CoordinateConverter(const Size &size, const Page &page, const ViewLoc &vloc)
		: size_(size)
		, page_(page)
		, vloc_(vloc) {
	}

	CoordinateConverter() = delete;
	CoordinateConverter(const CoordinateConverter &) = delete;
	CoordinateConverter(CoordinateConverter &&) = delete;
	CoordinateConverter &operator=(const CoordinateConverter &) = delete;
	CoordinateConverter &operator=(CoordinateConverter &&) = delete;

	// ��ͼ��ĳ����vRowLocǰ�������
	LineN lineOffset(const VRowLoc &vRowLoc) const;

private:
	const Size &size_;
	const Page &page_;
	const ViewLoc &vloc_;
};


}
