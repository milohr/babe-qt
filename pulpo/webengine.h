#ifndef WEBENGINE_H
#define WEBENGINE_H

#include <QWebEnginePage>
#include <QDebug>
#include <QObject>
#include <QEventLoop>
#include <QByteArray>

class webEngine : public QObject
{
    Q_OBJECT
public:
    explicit webEngine(QObject *parent = nullptr);
    void load(const QUrl &url);
private:
    QWebEnginePage *web_page_;

    QByteArray getHtml();
    QByteArray html;
signals:
void htmlReady(const QString &html);
public slots:
};

#endif // WEBENGINE_H
