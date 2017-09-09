#include "webengine.h"

webEngine::webEngine(QObject *parent) : QObject(parent)
{

}

void webEngine::load(const QUrl &url){

    qDebug()<<"trying to load url"<<url;

    QEventLoop loop;
    auto web_page_ = new QWebEnginePage(this);
    connect(web_page_, &QWebEnginePage::loadFinished, &loop, &QEventLoop::quit);
    web_page_->load(url);

    loop.exec();


    web_page_->toHtml([this](const QString  &contents)
    {
        this->html = contents.toLocal8Bit();
        emit htmlReady(contents);
    });

}

QByteArray webEngine::getHtml()
{
    return this->html;
}
