import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import client 1.0

Window {
    width: 640
    height: 320
    visible: true
    title: "Client"

    Client {
        id: connection
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 16

        ColumnLayout {
            spacing: 12
            Layout.preferredWidth: 200

            TextField {
                id: hostField
                placeholderText: "Host"
                text: "127.0.0.1"
            }

            TextField {
                id: portField
                placeholderText: "Port"
                text: "8080"
                inputMethodHints: Qt.ImhDigitsOnly
            }

            Button {
                text: connection.connected ? "Disconnect" : "Connect"
								onClicked: {
										if (connection.connected)
												connection.disconnect()
										else
												connection.connectTo(hostField.text, parseInt(portField.text))
								}
            }

            Text {
                text: connection.statusText
                color: connection.connected ? "green" : "red"
                wrapMode: Text.Wrap
            }
        }

        Rectangle {
            id: messageBox
            color: "#f4f4f4"
            radius: 6
            border.color: "#d0d0d0"
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: 160

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 8
                spacing: 8

                ListView {
                    id: messageList
                    model: connection.messages
                    clip: true
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.minimumHeight: 80
                    delegate: Text {
                        text: modelData
                        wrapMode: Text.Wrap
                    }
                }

                RowLayout {
                    id: inputRow
                    spacing: 8
                    Layout.fillWidth: true

                    TextField {
                        id: messageInput
                        placeholderText: "Enter a message..."
                        Layout.fillWidth: true
                    }

                    Button {
                        id: sendButton
                        text: "Send"
                        onClicked: {
                            connection.sendMessage(messageInput.text)
                            messageInput.text = ""
                        }
                    }
                }
            }
        }
    }
}
