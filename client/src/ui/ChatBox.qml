import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import ChatApp 1.0

Rectangle {
    id: root
    color: "#f4f4f4"
    radius: 6
    border.color: "#d0d0d0"

    Chat {
        id: chat
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 8

        ListView {
            id: messageList
            model: chat.messages
            clip: true
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: 80
            delegate: Text {
                text: modelData
                wrapMode: Text.Wrap
            }
        }

        RowLayout {
            id: inputRow
            spacing: 8
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
