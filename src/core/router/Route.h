#ifndef ROUTE_H
#define ROUTE_H

#include <QIcon>
#include <QString>
#include <QVariantMap>
#include <QWidget>
#include <functional>

class Route {
public:
    using Builder = std::function<QWidget*(const QVariantMap &params)>;

    Route(QString name, QString label, QIcon icon, Builder builder);

    QString name() const { return m_name; }
    QString label() const { return m_label; }
    QIcon icon() const { return m_icon; }

    QWidget* resolve(const QVariantMap &params);
    void invalidate();

private:
    bool shouldRebuild(const QVariantMap &params) const;

    QString m_name;
    QString m_label;
    QIcon m_icon;
    Builder m_builder;

    QWidget *m_cachedPage = nullptr;
    QVariantMap m_lastParams;
    bool m_hasParams = false;
};

#endif // ROUTE_H
