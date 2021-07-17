#include "charset_converter.h"

#include <cassert>
#include <memory>
#include <sstream>
#include <stdexcept>

#include <QTextDecoder>
#include <QTextEncoder>


namespace detail
{

class CharsetConverterImpl {
public:
    CharsetConverterImpl() {}

    ~CharsetConverterImpl() {}

    void open(Charset from, Charset to)
    {
        if (isOpened()) {
            throw std::logic_error("CharsetConverterImpl::open() error because already opened");
        }

        if (from == to) {
            isSameCharset_ = true;
            return;
        }

        const std::string sfrom = CharsetUtil::charsetToStr(from);
        QTextCodec *decoderCodec = QTextCodec::codecForName(sfrom.c_str());
        decoder_ = std::unique_ptr<QTextDecoder>(decoderCodec->makeDecoder());
        if (!decoder_) {
            std::ostringstream ss;
            ss << "CharsetConverterImpl::open() cannot create decoder for charset [" << sfrom << "]";
            throw std::runtime_error(ss.str());
        }

        const std::string sto = CharsetUtil::charsetToStr(to);
        QTextCodec *encoderCodec = QTextCodec::codecForName(sto.c_str());
        encoder_ = std::unique_ptr<QTextEncoder>(encoderCodec->makeEncoder());
        if (!encoder_) {
            std::ostringstream ss;
            ss << "CharsetConverterImpl::open() cannot create encoder for charset [" << sto << "]";
            throw std::runtime_error(ss.str());
        }
    }

    void convert(const void *data, size_t len, MemBuff &buff)
    {
        if (!isOpened()) {
            throw std::logic_error("CharsetConverterImpl::convert() error because converter not opened");
        }

        if (isSameCharset_) {
            buff.append(data, len);
            return;
        }

        if (data != nullptr || len > 0) {
            QString str = decoder_->toUnicode(reinterpret_cast<const char *>(data), static_cast<int>(len));
            QByteArray byteArray = encoder_->fromUnicode(str);
            buff.append(byteArray.constData(), byteArray.size());
        }
    }

private:
    bool isOpened() const {
        if (isSameCharset_) {
            return true;
        } else {
            return decoder_ && encoder_;
        }
    }

private:
    bool isSameCharset_ = false;
    std::unique_ptr<QTextDecoder> decoder_;
    std::unique_ptr<QTextEncoder> encoder_;
};

}

CharsetConverter::CharsetConverter()
{
    impl_ = new detail::CharsetConverterImpl;
}

CharsetConverter::~CharsetConverter()
{
    delete impl_;
    impl_ = nullptr;
}

void CharsetConverter::open(Charset from, Charset to)
{
    assert(impl_);
    impl_->open(from, to);
}

void CharsetConverter::convert(const void *data, size_t len, MemBuff &buff)
{
    assert(impl_);
    impl_->convert(data, len, buff);
}
