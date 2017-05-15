#ifndef BAEUTILS_H
#define BAEUTILS_H

#include <QApplication>
#include <QDebug>
#include <QFileInfo>
#include <QStandardPaths>
#include <QString>

using namespace std;

namespace BaeUtils
{
static inline QString getNameFromLocation(const QString &str)
{
    QString ret;
    int index = 0;
    for (int i = str.size() - 1; i >= 0; i--) {
        if (str[i] == '/') {
            index = i + 1;
            i = -1;
        }
    }
    for(; index < str.size(); index++)
        ret.push_back(str[index]);
    return ret;
}

static inline QString getSettingPath()
{
    return QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/" + QApplication::applicationName();
}

static inline QString getCollectionDBPath()
{
    return QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
}

static inline QString getCachePath()
{
    return QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation) + "/" + QApplication::applicationName();
}

static inline QString getYoutubeCachePath()
{
    return QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation) + "/" + QApplication::applicationName() + "/youtube/";
}

static inline QString getExtensionFetchingPath()
{
    return QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
}

static inline QString getNotifyDir()
{
    return QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
}

static inline QString fixTitle(const QString &title, const QString &s, const QString &e)
{
    QString newTitle;
    for (int i = 0; i < title.size(); i++) {
        if (title.at(i) == s) {
            while (title.at(i) != e) {
                if (i == title.size()-1)
                    break;
                else
                    i++;
            }
        } else {
            newTitle += title.at(i);
        }
    }
    return newTitle.simplified();
}

static inline QString removeSubstring(const QString &newTitle, const QString &subString)
{
    const int indexFt = newTitle.indexOf(subString, 0, Qt::CaseInsensitive);
    if (indexFt != -1)
        return newTitle.left(indexFt).simplified();
    return newTitle;
}

static inline QString ucfirst(const QString &str)
{
    if (str.isEmpty())
        return "";
    QString output;
    QStringList result;
    if (str.contains(" ")) {
        for (auto str : str.split(" ")) {
            str = str.toLower();
            str[0] = str[0].toUpper();
            result << str;
        }
        output = result.join(" ");
    } else {
        output = str;
    }
    return output.simplified();
}

static inline QString fixString(const QString &str)
{
    QString title = str;
    title = title.contains("(") && title.contains(")") ? fixTitle(title, "(",")") : title;
    title = title.contains("[") && title.contains("]") ? fixTitle(title, "[","]") : title;
    title = title.contains("{") && title.contains("}") ? fixTitle(title, "{","}") : title;
    title = title.contains("ft") ? removeSubstring(title, "ft") : title;
    title = title.contains("ft.") ? removeSubstring(title, "ft.") : title;
    title = title.contains("featuring") ? removeSubstring(title, "featuring") : title;
    title = title.contains("feat") ? removeSubstring(title, "feat") : title;
    title = title.contains("official video") ? removeSubstring(title, "official video") : title;
    title = title.contains("live") ? removeSubstring(title, "live") : title;
    title = title.contains("...") ? title.replace("...", "") : title;
    title = title.contains("|") ? title.replace("|", "") : title;
    title = title.contains('"') ? title.replace('"', "") : title;
    title = title.contains(":") ? title.replace(":", "") : title;
    title = title.contains("&") ? title.replace("&", "and") : title;
    return ucfirst(title).simplified();
}

static inline bool fileExists(const QString &url)
{
    return QFileInfo(url).exists();
}

static inline QStringList getMoodColors()
{
    return {"#F0FF01", "#01FF5B", "#3DAEFD", "#B401FF", "#E91E63"};
}
}

#endif // UTILS_H
