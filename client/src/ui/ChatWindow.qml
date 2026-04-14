import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import ChatApp 1.0

Window {
    id: root

    width: 920
    height: 560
    visible: true
    title: "Chat app"
    color: "#efe4d4"

    ConnectionWindow {
        id: connectionWindow
        transientParent: root
        x: root.x + Math.round((root.width - width) / 2)
        y: root.y + Math.round((root.height - height) / 2)
    }

    AccountWindow {
        id: accountWindow
        transientParent: root
        x: root.x + Math.round((root.width - width) / 2)
        y: root.y + Math.round((root.height - height) / 2)
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 16

        RowLayout {
            spacing: 16
            Layout.fillWidth: true

            Button {
                text: "Profile"
                onClicked: {
                    accountWindow.show()
                    accountWindow.raise()
                    accountWindow.requestActivate()
                }
            }

            Button {
                text: Client.connected ? "Disconnect" : "Connect"
                onClicked: {
                    if (Client.connected) {
                        Client.disconnect()
                    } else {
                        connectionWindow.show()
                        connectionWindow.raise()
                        connectionWindow.requestActivate()
                    }
                }
            }

            StatusIndicator { }
        }

        ChatBox {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: 220
        }
    }
}
