import QtQuick
import QtQuick.Controls

Item {
    id: root
    width: parent.width
    height: 50

    required property QtObject chat
    readonly property bool selected: ChatManager.selectedChat === root.chat

    Button {
        anchors.fill: parent
        text: root.chat.label
        highlighted: root.selected
        background: Rectangle {
            radius: 10
            color: root.selected ? "#ddcdb9" : "#fbf7f2"
            border.color: root.selected ? "#b79e84" : "#e4d8c7"
            border.width: 1
        }
        onClicked: {
            ChatManager.selectChat(root.chat.id)
        }
    }
}
