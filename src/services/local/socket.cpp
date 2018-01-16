
#include "socket.h"
#include <QWebSocketServer>
#include <QWebSocket>
#include <QDebug>

QT_USE_NAMESPACE

Socket::Socket(quint16 port, QObject *parent) :
    QObject(parent),
    m_pWebSocketServer(new QWebSocketServer(QStringLiteral("Babe Server"),
                                            QWebSocketServer::NonSecureMode, this))
{
    if (this->m_pWebSocketServer->listen(QHostAddress::Any, port))
    {

        qDebug() << "Babe listening on port" << port;
        connect(this->m_pWebSocketServer, &QWebSocketServer::newConnection,
                this, &Socket::onNewConnection);
        connect(this->m_pWebSocketServer, &QWebSocketServer::closed, this, &Socket::closed);
    }
}

Socket::~Socket()
{
    m_pWebSocketServer->close();
    qDeleteAll(m_clients.begin(), m_clients.end());
}

void Socket::sendMessageTo(const int &client, const QString &message)
{
    if(this->m_clients.isEmpty() && this->m_clients.size()>=client)
        return;

    auto s_client = this->m_clients.at(client);
    s_client->sendTextMessage(message);
}

void Socket::onNewConnection()
{
    qDebug()<<"trying new connection";
    QWebSocket *pSocket = m_pWebSocketServer->nextPendingConnection();

    connect(pSocket, &QWebSocket::textMessageReceived, this, &Socket::processTextMessage);
    connect(pSocket, &QWebSocket::binaryMessageReceived, this, &Socket::processBinaryMessage);
    connect(pSocket, &QWebSocket::disconnected, this, &Socket::socketDisconnected);

    m_clients << pSocket;

    emit this->connected(m_clients.size()-1);
}

void Socket::processTextMessage(const QString &message)
{
    emit this->message(message);
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());

    qDebug() << "Message received:" << message;
    if (pClient)
    {
        pClient->sendTextMessage(message);
    }
}

void Socket::processBinaryMessage(QByteArray message)
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());

    qDebug() << "Binary Message received:" << message;
    if (pClient) {
        pClient->sendBinaryMessage(message);
    }
}

void Socket::socketDisconnected()
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());

    qDebug() << "socketDisconnected:" << pClient;
    if (pClient) {
        emit this->disconnected(pClient->origin());
        m_clients.removeAll(pClient);
        pClient->deleteLater();
    }
}


