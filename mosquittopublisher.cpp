#include "mosquittopublisher.h"

#include <mosquitto.h>

#include <QByteArray>

MosquittoPublisher::MosquittoPublisher(QObject *parent) : QObject(parent)
{
    mosquitto_lib_init();
    setStatus("Ready");
}

MosquittoPublisher::~MosquittoPublisher()
{
    mosquitto_lib_cleanup();
}

void MosquittoPublisher::setStatus(const QString &s)
{
    if (m_status == s) return;
    m_status = s;
    emit statusChanged();
}

bool MosquittoPublisher::connectPublishDisconnect(const QString &host,
                                                  int port,
                                                  const QString &username,
                                                  const QString &password,
                                                  const QString &topic,
                                                  const QByteArray &payloadUtf8)
{
    mosquitto *m = mosquitto_new(nullptr, true, nullptr);
    if (!m) {
        setStatus("Error: Can't initialize library");
        return false;
    }

    if (!username.isEmpty()) {
        int rc = mosquitto_username_pw_set(
            m,
            username.toUtf8().constData(),
            password.toUtf8().constData()
        );
        if (rc != MOSQ_ERR_SUCCESS) {
            setStatus(QString("Error: Can't set username and password: %1").arg(mosquitto_strerror(rc)));
            mosquitto_destroy(m);
            return false;
        }
    }

    int rc = mosquitto_connect(m, host.toUtf8().constData(), port, /*keepalive*/ 20);
    if (rc != MOSQ_ERR_SUCCESS) {
        setStatus(QString("Connection error: %1").arg(mosquitto_strerror(rc)));
        mosquitto_destroy(m);
        return false;
    }

    int mid = 0;
    rc = mosquitto_publish(
        m,
        &mid,
        topic.toUtf8().constData(),
        payloadUtf8.size(),
        payloadUtf8.constData(),
        /*qos*/ 0,
        /*retain*/ false
    );
    if (rc != MOSQ_ERR_SUCCESS) {
        setStatus(QString("publish ошибка: %1").arg(mosquitto_strerror(rc)));
        mosquitto_disconnect(m);
        mosquitto_destroy(m);
        return false;
    }

    // Для QoS0 обычно достаточно send, но чтобы гарантировать уход пакета — сделаем короткий loop
    rc = mosquitto_loop(m, /*timeout ms*/ 200, /*max_packets*/ 1);
    if (rc != MOSQ_ERR_SUCCESS) {
        // не фатально, но покажем
        setStatus(QString("loop предупреждение: %1").arg(mosquitto_strerror(rc)));
    } else {
        setStatus(QString("Опубликовано (%1 байт), mid=%2").arg(payloadUtf8.size()).arg(mid));
    }

    mosquitto_disconnect(m);
    mosquitto_destroy(m);
    return true;
}

void MosquittoPublisher::publishMessage(const QString &host,
                                        int port,
                                        const QString &username,
                                        const QString &password,
                                        const QString &topic,
                                        const QString &payload)
{
    const QString h = host.trimmed();
    const QString t = topic.trimmed();

    if (h.isEmpty() || t.isEmpty()) {
        setStatus("Адрес и топик обязательны");
        return;
    }
    if (port <= 0 || port > 65535) {
        setStatus("Некорректный порт");
        return;
    }

    setStatus("Отправка...");
    connectPublishDisconnect(h, port, username, password, t, payload.toUtf8());
}
