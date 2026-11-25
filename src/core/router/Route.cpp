#include "Route.h"

Route::Route(QString name, QString label, QIcon icon, Builder builder)
    : m_name(std::move(name))
    , m_label(std::move(label))
    , m_icon(std::move(icon))
    , m_builder(std::move(builder)) {}

bool Route::shouldRebuild(const QVariantMap &params) const {
    if (!m_cachedPage) {
        return true;
    }
    if (!m_hasParams) {
        return !params.isEmpty();
    }
    return m_lastParams != params;
}

QWidget* Route::resolve(const QVariantMap &params) {
    if (!shouldRebuild(params)) {
        return m_cachedPage;
    }

    if (m_cachedPage) {
        QWidget *oldPage = m_cachedPage;
        m_cachedPage = nullptr;
        oldPage->deleteLater();
    }

    if (!m_builder) {
        return nullptr;
    }

    QWidget *newPage = m_builder(params);
    if (!newPage) {
        return nullptr;
    }

    m_cachedPage = newPage;
    m_lastParams = params;
    m_hasParams = true;

    QObject::connect(newPage, &QObject::destroyed, newPage, [this, newPage]() {
        if (m_cachedPage == newPage) {
            m_cachedPage = nullptr;
            m_hasParams = false;
        }
    });

    return m_cachedPage;
}

void Route::invalidate() {
    if (m_cachedPage) {
        QWidget *oldPage = m_cachedPage;
        m_cachedPage = nullptr;
        oldPage->deleteLater();
    }
    m_hasParams = false;
}
