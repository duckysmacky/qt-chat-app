import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import ChatApp 1.0

Window {
    width: 360
    height: 420
    visible: false
    title: "Connect"
    color: "#efe4d4"
    modality: Qt.WindowModal
    flags: Qt.Dialog

    ConnectionPanel {
        anchors.fill: parent
        anchors.margins: 20
    }

    onVisibleChanged: {
        if (visible)
            requestActivate()
    }

    Connections {
        target: Client

        function onConnectionStatusChanged() {
            if (Client.connected)
                close()
        }
    }
}
