#pragma once

#include "size.h"
#include "page.h"
#include "view_loc.h"
#include "line_offset.h"
#include "declare_text_area_config.h"


namespace gui
{

// �Ѳ�ͬ����ϵ�е�λ���໥ת��
// ����������ϵ���ĵ����ꡢ��ͼ���ꡢGUI��������
// �ⲿ���߼�����TextArea��������Ϊ���ƺ���̫����ʵ�ִ���ϳ������Ե�������
class CoordinateConverter {
public:
	CoordinateConverter(const Size &size, const Page &page, const ViewLoc &vloc, const TextAreaConfig &config)
		: size_(size)
		, page_(page)
		, vloc_(vloc)
		, config_(config) {
	}

	CoordinateConverter() = delete;
	CoordinateConverter(const CoordinateConverter &) = delete;
	CoordinateConverter(CoordinateConverter &&) = delete;
	CoordinateConverter &operator=(const CoordinateConverter &) = delete;
	CoordinateConverter &operator=(CoordinateConverter &&) = delete;

	// ��ͼ��ĳ����vRowLocǰ�������
	LineOffset lineOffset(const VRowLoc &vRowLoc) const;

	// ��ͼ��ĳ�е��ı�����������
	int baselineY(LineOffset lineOffset) const;

private:
	const Size &size_;
	const Page &page_;
	const ViewLoc &vloc_;
	const TextAreaConfig &config_;
};


}
