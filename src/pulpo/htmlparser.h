#ifndef HTMLPARSER_H
#define HTMLPARSER_H
#include <QObject>
#include <QStringList>
#include <QString>
#include <QDebug>

class htmlParser : public QObject
{
    Q_OBJECT
public:
    explicit htmlParser(QObject *parent = nullptr);
    void setHtml(const QByteArray &html);
    QStringList parseTag(const QString &tagRef, const QString &attribute=""); // return all tag matches with content
    bool findTag(const QString &txt, const QString &tagRef);
    QString extractProp(const QString &tag, const QString &prop);


private:
    QByteArray html;

signals:
    void finishedParsingTags(const QStringList &tags);
    void finishedExtractingProp(const QString &prop);

public slots:
};

#endif // HTMLPARSER_H
