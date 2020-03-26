#ifndef MANGACHAPTER_H
#define MANGACHAPTER_H
#include <QtCore>

class MangaChapter
{
public:
    QString chapterTitle;
    QString chapterUrl;

    bool pagesLoaded;
    int numPages;

    QStringList pageUrlList;
    QStringList imageUrlList;

    explicit MangaChapter(const QString &title, const QString &url);
    MangaChapter();
};

QDataStream &operator<<(QDataStream &str, const MangaChapter &m);
QDataStream &operator>>(QDataStream &str, MangaChapter &m);

#endif  // MANGACHAPTER_H
