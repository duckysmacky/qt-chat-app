import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import ChatApp 1.0

Rectangle {
    color: "#f7efe5"
    radius: 16
    border.color: "#ddcdb9"
    border.width: 1

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 18
        spacing: 14

        Label {
            text: "Profile"
            font.pixelSize: 22
            font.bold: true
            color: "#2f241f"
        }

        Label {
            text: AccountManager.currentUser.length > 0
                  ? "Logged in as " + AccountManager.currentUser
                  : "Logged in"
            color: "#4f4038"
            wrapMode: Text.Wrap
            Layout.fillWidth: true
        }

        Item {
            Layout.fillHeight: true
        }

        Button {
            text: "Log out"
            Layout.fillWidth: true
            onClicked: AccountManager.logout()
        }
    }
}
