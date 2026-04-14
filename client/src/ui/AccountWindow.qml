import QtQuick
import QtQuick.Controls

import ChatApp 1.0

Window {
    width: 380
    height: 500
    visible: false
    title: "Profile"
    color: "#efe4d4"
    modality: Qt.WindowModal
    flags: Qt.Dialog

    Loader {
        anchors.fill: parent
        anchors.margins: 20
        sourceComponent: AccountManager.mode === AccountManager.RegisterMode
                         ? registerComponent
                         : AccountManager.mode === AccountManager.AccountMode
                           ? profileComponent
                           : loginComponent
    }

    Component {
        id: loginComponent
        LoginPanel { }
    }

    Component {
        id: registerComponent
        RegisterPanel { }
    }

    Component {
        id: profileComponent
        ProfilePanel { }
    }

    onVisibleChanged: {
        if (visible)
            requestActivate()
    }
}
