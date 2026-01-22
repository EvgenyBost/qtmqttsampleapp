#pragma once

#include <QObject>
#include <QString>

struct mosquitto;

class MosquittoPublisher : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)

public:
    explicit MosquittoPublisher(QObject *parent = nullptr);

    ~MosquittoPublisher() override;

        QString status() const { return m_status; }

        Q_INVOKABLE void publishMessage(const QString &host,
                                       int port,
                                       const QString &username,
                                       const QString &password,
                                       const QString &topic,
                                       const QString &payload);

    signals:
        void statusChanged();

    private:
        QString m_status;

        void setStatus(const QString &s);

        // Just one publish, then disconnect: "connect -> publish -> disconnect"
        bool connectPublishDisconnect(const QString &host,
                                     int port,
                                     const QString &username,
                                     const QString &password,
                                     const QString &topic,
                                     const QByteArray &payloadUtf8);
};

