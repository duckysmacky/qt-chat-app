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

        ColumnLayout {
            spacing: 4

            Label {
                text: "Login"
                font.pixelSize: 22
                font.bold: true
                color: "#2f241f"
            }

            Label {
                text: "Connect to the server, then sign in to send messages"
                color: "#6a564d"
                wrapMode: Text.Wrap
                Layout.fillWidth: true
            }
        }

        ColumnLayout {
            spacing: 10
            Layout.fillWidth: true

            Label {
                text: "Username"
                color: "#4f4038"
            }

            TextField {
                id: loginField
                Layout.fillWidth: true
                enabled: Client.connected && !AccountManager.busy
            }

            Label {
                text: "Password"
                color: "#4f4038"
            }

            TextField {
                id: passwordField
                Layout.fillWidth: true
                echoMode: TextInput.Password
                enabled: Client.connected && !AccountManager.busy
                onAccepted: loginButton.clicked()
            }
        }

        Item {
            Layout.fillHeight: true
        }

        Button {
            id: loginButton
            text: AccountManager.busy ? "Logging in..." : "Login"
            Layout.fillWidth: true
            enabled: Client.connected && !AccountManager.busy
            onClicked: AccountManager.login(loginField.text, passwordField.text)
        }

        Button {
            text: "Create an account"
            Layout.fillWidth: true
            flat: true
            enabled: !AccountManager.busy
            onClicked: AccountManager.showRegister()
        }
    }
}
