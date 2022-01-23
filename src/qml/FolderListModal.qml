import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.5 as Kirigami

Kirigami.OverlaySheet {
    id: folderList

    ListView {
        anchors.fill: parent

        model: session.folderListModel.folderList

        delegate: Kirigami.BasicListItem {
            label: modelData

            onClicked: {
                session.messageListModel.moveMessages(
                            session.folderListModel.currentAccountName,
                            modelData)

                folderList.close()
            }
        }
    }

    Component.onCompleted: {

    }
}
