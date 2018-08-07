#pragma once

class NullableClass
{
public:
    bool isNull() const { return m_isNull; }
    void setNull(bool isNull) { m_isNull = isNull; }
protected:
    NullableClass() {}
    explicit NullableClass(bool isNull) :m_isNull(isNull) {}

private:
    bool m_isNull = true;
};
