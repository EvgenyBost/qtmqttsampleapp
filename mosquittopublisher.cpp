#include "mosquittopublisher.h"

#include <mosquitto.h>

#include <QByteArray>

#include <QRegularExpression>

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
        setStatus(QString("Publish error: %1").arg(mosquitto_strerror(rc)));
        mosquitto_disconnect(m);
        mosquitto_destroy(m);
        return false;
    }

    // Add a short loop after send to ensure packet delivery for QoS0
    rc = mosquitto_loop(m, /*timeout ms*/ 200, /*max_packets*/ 1);
    if (rc != MOSQ_ERR_SUCCESS) {
        setStatus(QString("Publish loop warning: %1").arg(mosquitto_strerror(rc)));
    } else {
        setStatus(QString("Published (%1 bytes), mid=%2").arg(payloadUtf8.size()).arg(mid));
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
        setStatus("Please provide Address and Topic");
        return;
    }

    // Check if Host address is correct
    QRegularExpression re("^(?:[a-z0-9](?:[a-z0-9-]{0,61}[a-z0-9])?\\.)+[a-z0-9][a-z0-9-]{0,61}[a-z0-9]$|[0-9]{1,3}(\\.[0-9]{1,3}){3}");
    QRegularExpressionMatch match = re.match(h);

    if (!match.hasMatch()) {
        // incorrect host
        setStatus("Error: incorrect Host Address");
        return;
    }

    // Topic field can't contain '+' or '#' characters
    if (topic.contains('+') || topic.contains('#'))
    {
        setStatus("Error: incorrect Topic. Please remove '+' or '#' characters");
        return;
    }

    if (port <= 0 || port > 65535) {
        setStatus("Wrong Port");
        return;
    }

    setStatus("Sending...");
    connectPublishDisconnect(h, port, username, password, t, payload.toUtf8());
}
