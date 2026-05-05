import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import ChatApp 1.0

Rectangle {
    signal closeRequested()

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

        GridLayout {
            columns: 2
            columnSpacing: 14
            rowSpacing: 8
            Layout.fillWidth: true

            Label {
                text: "User ID"
                color: "#6d5b50"
                font.bold: true
                Layout.alignment: Qt.AlignTop
            }

            TextArea {
                text: AccountManager.userProfileLoaded ? AccountManager.profileUserId : "Loading..."
                readOnly: true
                selectByMouse: true
                color: "#2f241f"
                wrapMode: Text.WrapAnywhere
                Layout.fillWidth: true
                background: null
                padding: 0
            }

            Label {
                text: "Username"
                color: "#6d5b50"
                font.bold: true
            }

            TextArea {
                text: AccountManager.userProfileLoaded ? AccountManager.profileUsername : "Loading..."
                readOnly: true
                selectByMouse: true
                color: "#2f241f"
                wrapMode: Text.Wrap
                Layout.fillWidth: true
                background: null
                padding: 0
            }

            Label {
                text: "Name"
                color: "#6d5b50"
                font.bold: true
            }

            TextArea {
                text: AccountManager.userProfileLoaded ? AccountManager.profileName : "Loading..."
                readOnly: true
                selectByMouse: true
                color: "#2f241f"
                wrapMode: Text.Wrap
                Layout.fillWidth: true
                background: null
                padding: 0
            }

            Label {
                text: "Email"
                color: "#6d5b50"
                font.bold: true
            }

            TextArea {
                text: AccountManager.userProfileLoaded ? AccountManager.profileEmail : "Loading..."
                readOnly: true
                selectByMouse: true
                color: "#2f241f"
                wrapMode: Text.WrapAnywhere
                Layout.fillWidth: true
                background: null
                padding: 0
            }
        }

        Item {
            Layout.fillHeight: true
        }

        Button {
            text: "Log out"
            Layout.fillWidth: true
            onClicked: AccountManager.logout()
        }

        Button {
            text: "Close"
            Layout.fillWidth: true
            onClicked: closeRequested()
        }
    }
}
