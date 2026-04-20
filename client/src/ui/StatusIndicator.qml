import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import ChatApp 1.0

RowLayout {
		spacing: 8

		Rectangle {
				width: 10
				height: 10
				radius: 5
				color: Client.connected ? "#2d8a45" : "#c45454"
				Layout.alignment: Qt.AlignVCenter
		}

		Label {
				text: Client.connected ? "Online" : "Offline"
				font.bold: true
				color: "#2f241f"
				Layout.alignment: Qt.AlignVCenter
		}
}
