import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3

Window {
    width: 520
    height: 520
    visible: true
    title: "MQTT Publisher"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 10

        GridLayout {
            columns: 2
            columnSpacing: 10
            rowSpacing: 10
            Layout.fillWidth: true

            Label { text: "Address:" }
            TextField { id: hostField; text: "test.mosquitto.org"; Layout.fillWidth: true; }

            Label { text: "Port:" }
            SpinBox { id: portField; from: 1; to: 65535; value: 1883; editable: true; Layout.fillWidth: true ; stepSize: 1;
                        // Delete ',' from number > 999
                        textFromValue: function(value, locale) {
                            return value.toString();
                        }

                        valueFromText: function(text, locale) {
                            return parseInt(text);
                        }
            }

            Label { text: "Username:" }
            TextField { id: userField; Layout.fillWidth: true }

            Label { text: "Password:" }
            TextField { id: passField; echoMode: TextInput.Password; Layout.fillWidth: true }

            Label { text: "Topic:" }
            TextField { id: topicField; text: "boss/test/topic"; Layout.fillWidth: true }

            Label { text: "Payload:"; Layout.alignment: Qt.AlignTop }
            TextArea {
                id: payloadField
                Layout.fillWidth: true
                Layout.preferredHeight: 160
                wrapMode: TextArea.Wrap
                placeholderText: "Text/JSON"
                text: "Test message"
            }
        }

        Button {
            text: "Send"
            Layout.fillWidth: true
            onClicked: mqttPublisher.publishMessage(
                           hostField.text,
                           portField.value,
                           userField.text,
                           passField.text,
                           topicField.text,
                           payloadField.text
                       )
        }

        Label {
            Layout.fillWidth: true
            text: "Status: " + mqttPublisher.status
            wrapMode: Text.WordWrap
        }

        Item { Layout.fillHeight: true }
    }
}
