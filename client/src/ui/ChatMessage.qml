import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    required property QtObject message

    implicitWidth: ListView.view ? ListView.view.width : 0
    implicitHeight: bubble.implicitHeight

    Rectangle {
        id: bubble

        readonly property real maxBubbleWidth: root.width * 0.65
        readonly property int horizontalPadding: 14
        readonly property int verticalPadding: 10

        x: root.message.isOwn ? root.width - width : 0
        width: Math.min(maxBubbleWidth, contentColumn.implicitWidth + horizontalPadding * 2)
        implicitHeight: contentColumn.implicitHeight + verticalPadding * 2
        radius: 18
        color: root.message.isOwn ? "#d8f5c6" : "#ffffff"
        border.color: root.message.isOwn ? "#bddfb1" : "#e0d7ca"
        border.width: 1

        ColumnLayout {
            id: contentColumn

            anchors.fill: parent
            anchors.leftMargin: bubble.horizontalPadding
            anchors.rightMargin: bubble.horizontalPadding
            anchors.topMargin: bubble.verticalPadding
            anchors.bottomMargin: bubble.verticalPadding
            spacing: 6

            Label {
                visible: !root.message.isOwn && text.length > 0
                text: root.message.sender
                color: "#7a6d5b"
                opacity: 0.7
                font.pixelSize: 12
                Layout.fillWidth: true
            }

            Text {
                text: root.message.content
                color: "#2f241f"
                wrapMode: Text.Wrap
                font.pixelSize: 15
                Layout.fillWidth: true
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 8

                Label {
                    text: root.message.isOwn ? root.message.timeSent : root.message.timeReceived
                    visible: text.length > 0
                    color: "#7a6d5b"
                    opacity: 0.7
                    font.pixelSize: 12
                }

                Item {
                    Layout.fillWidth: true
                    visible: root.message.isOwn
                }

                Label {
                    text: root.message.isOwn ? root.message.statusText : ""
                    visible: root.message.isOwn && text.length > 0
                    color: "#7a6d5b"
                    opacity: 0.7
                    font.pixelSize: 12
                    horizontalAlignment: Text.AlignRight
                }
            }
        }
    }
}
