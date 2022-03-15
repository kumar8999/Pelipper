import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.5 as Kirigami

Kirigami.OverlaySheet {
    id: folderList

    TreeView {
        id: treeView
        Layout.fillHeight: true
        Layout.fillWidth: true
        headerVisible: false
        model: session.folderListModel
        alternatingRowColors: false
        frameVisible: false
        anchors.margins: 0

        //    backgroundVisible: false
        TableViewColumn {
            title: "Name"
            role: "display"
        }

        BusyIndicator {
            anchors.centerIn: parent
            visible: session.folderListModel.loading
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

    Component.onCompleted: {

    }
}
