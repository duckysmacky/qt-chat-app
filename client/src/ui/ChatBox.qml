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
    readonly property Chat currentChat: ChatManager.selectedChat

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 14
        visible: root.currentChat !== null

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
                model: root.currentChat ? root.currentChat.messages : []
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
                placeholderText: root.currentChat && AccountManager.canSendMessages
                                 ? "Enter a message..."
                                 : ""
                Layout.fillWidth: true
                enabled: root.currentChat !== null && AccountManager.canSendMessages
                onAccepted: {
                    sendButton.clicked()
                }
            }

            Button {
                id: sendButton
                text: "Send"
                enabled: root.currentChat !== null && AccountManager.canSendMessages
                onClicked: {
                    if (!root.currentChat)
                        return

                    root.currentChat.submitMessage(messageInput.text)
                    messageInput.text = ""
                }
            }
        }
    }
}
