#pragma once

#include <sstream>
#include <string>

#include <boost/core/demangle.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include "core/basetype.h"


namespace msg
{

template <typename T>
inline std::string typeName() {
    return boost::core::demangle(typeid(T).name());
}

struct Ok {
    template <typename Ar>
    void serialize(Ar &ar, unsigned version) {}
};

struct ActivateUI {
    template <typename Ar>
    void serialize(Ar &ar, unsigned version) {}
};

struct OpenFiles {

    struct OpenInfo {
        u8str u8file;
        i64 row;
        template <typename Ar>
        void serialize(Ar &ar, unsigned version) {
            ar & static_cast<std::string &>(u8file);
            ar & row;
        }
    };

    std::vector<OpenInfo> files;

    template <typename Ar>
    void serialize(Ar &ar, unsigned version) {
        ar & files;
    }
};

class Pack {
public:
    Pack() {}

    template <typename Msg>
    Pack(Msg &&msg)
        : type_(typeName<Msg>()) {

        std::ostringstream ss;
        boost::archive::text_oarchive ar(ss);
        ar << msg;
        data_ = ss.str();
    }

    template <typename Msg>
    bool is() const {
        return typeName<Msg>() == type_;
    }

    class Unpacker {
    public:
        Unpacker(const std::string &data)
            : ss_(data) {}

        template <typename Msg>
        operator Msg() {
            boost::archive::text_iarchive ar(ss_);
            Msg msg;
            ar >> msg;
            return msg;
        }

    private:
        std::istringstream ss_;
    };

    Unpacker unpack() const {
        return Unpacker(data_);
    }

    std::string serialize() const {
        std::ostringstream ss;
        boost::archive::text_oarchive ar(ss);
        ar << *this;
        return ss.str();
    }

    static Pack deserialize(const std::string &data) {
        std::istringstream ss(data);
        boost::archive::text_iarchive ar(ss);
        Pack pack;
        ar >> pack;
        return pack;
    }

private:

    template <typename Ar>
    void serialize(Ar &ar, unsigned version) {
        ar & type_;
        ar & data_;
    }
    
private:
    std::string type_;
    std::string data_;

    friend class boost::serialization::access;
};

}
