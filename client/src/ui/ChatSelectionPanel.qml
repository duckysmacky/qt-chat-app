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

    Rectangle {
        anchors.fill: parent
        anchors.margins: 18
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.minimumHeight: 120
        Layout.minimumWidth: 60
        radius: 12
        color: "#fbf7f2"
        border.color: "#e4d8c7"

        ListView {
            id: chatCardList
            anchors.fill: parent
            anchors.margins: 12
            model: ChatManager.chats
            clip: true
            spacing: 8

            onCountChanged: positionViewAtEnd()

            delegate: ChatCard {
                required property var modelData
                width: ListView.view.width
                chat: modelData
            }
        }
    }
}
