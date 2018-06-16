#pragma once

#include <QFontMetrics>

class FontInfoProvider
{
public:
    FontInfoProvider(const QFontMetrics &metrics) : m_metrics(metrics) {}



private:
    const QFontMetrics &m_metrics;
};
