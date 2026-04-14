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
                text: "Create an account"
                font.pixelSize: 22
                font.bold: true
                color: "#2f241f"
            }

            Label {
                text: "Registration uses the existing server-side account system"
                color: "#6a564d"
                wrapMode: Text.Wrap
                Layout.fillWidth: true
            }
        }

        ColumnLayout {
            spacing: 10
            Layout.fillWidth: true

            Label { text: "Username"; color: "#4f4038" }
            TextField {
                id: usernameField
                Layout.fillWidth: true
                enabled: Client.connected && !AccountManager.busy
            }

            Label { text: "Name"; color: "#4f4038" }
            TextField {
                id: nameField
                Layout.fillWidth: true
                enabled: Client.connected && !AccountManager.busy
            }

            Label { text: "Email"; color: "#4f4038" }
            TextField {
                id: emailField
                Layout.fillWidth: true
                enabled: Client.connected && !AccountManager.busy
            }

            Label { text: "Password"; color: "#4f4038" }
            TextField {
                id: passwordField
                Layout.fillWidth: true
                echoMode: TextInput.Password
                enabled: Client.connected && !AccountManager.busy
                onAccepted: registerButton.clicked()
            }

            Label {
                text: AccountManager.statusText
                visible: text.length > 0
                color: "#6a564d"
                wrapMode: Text.Wrap
                Layout.fillWidth: true
            }
        }

        Item {
            Layout.fillHeight: true
        }

        Button {
            id: registerButton
            text: AccountManager.busy ? "Registering..." : "Register"
            Layout.fillWidth: true
            enabled: Client.connected && !AccountManager.busy
            onClicked: AccountManager.registerAccount(
                usernameField.text,
                nameField.text,
                emailField.text,
                passwordField.text
            )
        }

        Button {
            text: "Back to login"
            Layout.fillWidth: true
            flat: true
            enabled: !AccountManager.busy
            onClicked: AccountManager.showLogin()
        }
    }
}
