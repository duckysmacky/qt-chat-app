import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import ChatApp 1.0

Window {
    id: root

    width: 460
    height: 380
    visible: false
    title: "Create chat"
    color: "#efe4d4"
    modality: Qt.WindowModal
    flags: Qt.Dialog

    Rectangle {
        anchors.fill: parent
        anchors.margins: 16
        radius: 12
        color: "#fbf7f2"
        border.color: "#ddcdb9"
        border.width: 1

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 16
            spacing: 12

            RowLayout {
                Layout.fillWidth: true
                spacing: 8

                TextField {
                    id: userIdInput
                    Layout.fillWidth: true
                    placeholderText: "User UUID"
                    enabled: !ChatCreator.resolving
                    selectByMouse: true
                    onAccepted: {
                        if (addUserButton.enabled) {
                            addUserButton.clicked()
                        }
                    }
                }

                Button {
                    id: addUserButton
                    Layout.preferredWidth: 96
                    text: "Add user"
                    enabled: userIdInput.text.trim().length > 0 && !ChatCreator.resolving
                    onClicked: {
                        ChatCreator.addUser(userIdInput.text)
                        userIdInput.clear()
                    }
                }
            }

            Label {
                Layout.fillWidth: true
                text: ChatCreator.statusText
                visible: text.length > 0
                color: ChatCreator.statusText === "invalid user ID" ? "#a33a2d" : "#486447"
                elide: Text.ElideRight
            }

            Label {
                Layout.fillWidth: true
                text: "Chat members"
                font.bold: true
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumHeight: 120
                radius: 8
                color: "#ffffff"
                border.color: "#e4d8c7"

                ListView {
                    id: memberList
                    anchors.fill: parent
                    anchors.margins: 10
                    model: ChatCreator.members
                    clip: true
                    spacing: 8

                    delegate: Column {
                        required property var modelData

                        width: ListView.view.width
                        spacing: 2

                        Label {
                            width: parent.width
                            text: modelData.username
                            font.bold: true
                            elide: Text.ElideRight
                        }

                        Label {
                            width: parent.width
                            text: modelData.userId
                            color: "#6f6257"
                            font.pixelSize: 12
                            elide: Text.ElideMiddle
                        }
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 8

                Item {
                    Layout.fillWidth: true
                }

                Button {
                    text: "Create chat"
                    enabled: ChatCreator.canCreateChat
                    onClicked: {
                        ChatCreator.createChat()
                        root.close()
                    }
                }

                Button {
                    text: "Close"
                    onClicked: root.close()
                }
            }
        }
    }
}
