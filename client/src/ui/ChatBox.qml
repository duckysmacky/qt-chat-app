import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import ChatApp 1.0

Rectangle {
    id: root
    color: "#f7efe5"
    radius: 16
    border.color: "#ddcdb9"
    border.width: 1

    Chat {
        id: chat
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 18
        spacing: 14

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: 120
            radius: 12
            color: "#fbf7f2"
            border.color: "#e4d8c7"

            ListView {
                id: messageList
                anchors.fill: parent
                anchors.margins: 12
                model: chat.messages
                clip: true
                spacing: 6

                delegate: Rectangle {
                    required property string modelData

                    width: ListView.view.width
                    radius: 10
                    color: "#efe4d4"
                    border.color: "#e0cfba"

                    implicitHeight: messageText.implicitHeight + 14

                    Text {
                        id: messageText
                        anchors.fill: parent
                        anchors.margins: 6
                        text: modelData
                        wrapMode: Text.Wrap
                        color: "#3c2f29"
                    }
                }
            }
        }

        RowLayout {
            id: inputRow
            spacing: 5
            Layout.fillWidth: true

            TextField {
                id: messageInput
                placeholderText: "Enter a message..."
                Layout.fillWidth: true
                onAccepted: {
                    sendButton.clicked()
                }
            }

            Button {
                id: sendButton
                text: "Send"
                onClicked: {
                    chat.sendMessage(messageInput.text)
                    messageInput.text = ""
                }
            }
        }
    }
}
