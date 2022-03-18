import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.5 as Kirigami

Kirigami.OverlaySheet {
    id: folderList

    property string accountName: ""

    ListView {
        id: folderListView

        model: session.folderListModel.folderList

        delegate: Kirigami.BasicListItem {
            text: modelData

            onClicked: {
                session.messageListModel.moveMessages(modelData)
            }
        }
    }

    onAccountNameChanged: {
        console.log("on account name changed")
        session.folderListModel.loadFolderList(accountName)
    }
}
