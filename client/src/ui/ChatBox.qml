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
                spacing: 8

                onCountChanged: positionViewAtEnd()

                delegate: ChatMessage {
                    required property var modelData
                    width: ListView.view.width
                    message: modelData
                }
            }
        }

        RowLayout {
            id: inputRow
            spacing: 5
            Layout.fillWidth: true

            TextField {
                id: messageInput
                placeholderText: AccountManager.canSendMessages
                                 ? "Enter a message..."
                                 : "Log in to send messages"
                Layout.fillWidth: true
                enabled: AccountManager.canSendMessages
                onAccepted: {
                    sendButton.clicked()
                }
            }

            Button {
                id: sendButton
                text: "Send"
                enabled: AccountManager.canSendMessages
                onClicked: {
                    chat.submitMessage(messageInput.text)
                    messageInput.text = ""
                }
            }
        }
    }
}
