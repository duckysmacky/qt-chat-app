import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import ChatApp 1.0

Window {
    width: 920
    height: 560
    visible: true
    title: "Chat app"
    color: "#efe4d4"

    RowLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20

        ConnectionPanel {
            Layout.preferredWidth: 260
            Layout.fillHeight: true
        }

        ChatBox {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: 220
        }
    }
}
