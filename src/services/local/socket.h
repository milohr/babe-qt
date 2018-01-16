#ifndef SOCKET_H
#define SOCKET_H

#include <QObject>
#include <QList>
#include <QByteArray>

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)

class Socket : public QObject
{
    Q_OBJECT
public:
    explicit Socket(quint16 port, QObject *parent = Q_NULLPTR);
    ~Socket();
    void sendMessageTo(const int &client, const QString &message);

Q_SIGNALS:
    void closed();
    void disconnected(const QString &id);
    void connected(const int &index);
    void message(const QString &message);

private Q_SLOTS:
    void onNewConnection();
    void processTextMessage(const QString &message);
    void processBinaryMessage(QByteArray message);
    void socketDisconnected();

private:
    QWebSocketServer *m_pWebSocketServer;
    QList<QWebSocket *> m_clients;
};

#endif // SOCKET_H
