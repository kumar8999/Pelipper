import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.12
import org.kde.kirigami 2.13 as Kirigami

TreeView {
    Layout.fillHeight: true
    Layout.fillWidth: true
    id: treeView
    headerVisible: false
    model: session.folderListModel

    TableViewColumn {
        title: "Name"
        role: "display"
    }

    onDoubleClicked: function (index) {
        session.folderListModel.selectFolder(index)
    }

    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent
        width: parent.width

        visible: session.folderListModel.loading
        text: "loading..."
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.RightButton
        onClicked: {
            folderContextMenu.popup()
        }
        onPressAndHold: {
            if (mouse.source === Qt.MouseEventNotSynthesized)
                folderContextMenu.popup()
        }

        Menu {
            id: folderContextMenu
            MenuItem {
                enabled: session.folderListModel.loading
                text: qsTr("Reload")
            }

            MenuItem {
                enabled: session.folderListModel.loading
                text: qsTr("Check For New Messages")
            }

            MenuItem {
                enabled: session.folderListModel.loading
                text: qsTr("Delete")
            }

            MenuItem {
                enabled: session.folderListModel.loading
                text: qsTr("Create new child mailbox")
            }
        }
    }

    function expandRow() {
        var indexes = session.folderListModel.rowCount()

        for (var i = 0; i < indexes; i++) {
            treeView.expand(session.folderListModel.index(i, 0))
        }
    }

    Component.onCompleted: {
        session.folderListModel.loadingChanged.connect(expandRow)
    }
}
