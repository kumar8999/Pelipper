import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.12
import org.kde.kirigami 2.17 as Kirigami

ListView {
    property int mulBegin: 0
    property string selectedAccount: ""
    property var accountObj: {

    }

    id: messageListView
    model: session.messageListModel
    snapMode: ListView.SnapToItem
    headerPositioning: ListView.OverlayHeader
    highlightRangeMode: ListView.StrictlyEnforceRange
    ScrollBar.vertical: ScrollBar {}

    header: TextField {
        z: 3
        width: parent.width
        placeholderText: qsTr("Search...")
        visible: count > 0
    }

    BusyIndicator {
        anchors.centerIn: parent
        visible: session.messageListModel.loading
    }

    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent
        width: parent.width
        visible: !session.messageListModel.loading && count === 0
        text: "No Messages"
    }

    delegate: MessageItemDelegate {
        id: listItem
        width: messageListView.width

        highlightItem: model.Selected
        sender: model.Sender
        dateString: parseDate(model.Date)
        subject: model.Subject
        seenflag: model.Seen
        recentflag: model.Recent
        email: model.Email

        onItemClicked: {
            var i

            if (mouse.modifiers & Qt.ControlModifier) {
                messageListView.model.toggleSelected(model.index)
                messageListView.mulBegin = index

                let len = accountObj[model.Email]

                if (messageListView.model.isSelected(model.index)) {
                    len = len + 1
                } else {
                    len = len - 1
                }
                accountObj[model.Email] = len

                if (len === 0) {
                    delete accountObj[model.Email]
                }
            } else if (mouse.modifiers & Qt.ShiftModifier) {
                let len = accountObj[model.Email]

                if (index > messageListView.mulBegin) {
                    for (i = messageListView.mulBegin; i <= index; i++) {
                        messageListView.model.setSelected(i)
                        len = len + 1
                    }
                } else {
                    for (i = index; i <= messageListView.mulBegin; i++) {
                        messageListView.model.setSelected(i)
                        len = len + 1
                    }
                }
                accountObj[model.Email] = len
            } else {
                messageListView.model.clearSelections()
                messageListView.model.toggleSelected(model.index)

                accountObj = {}

                if (model.Selected) {
                    messageListView.mulBegin = index
                    accountObj[model.Email] = 1
                }

                session.messageListModel.setSeenFlag(index)
                session.selectedMessage(model.Email, model.Folder, model.Uid)
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.RightButton
        onClicked: {
            contextMenu.popup()
        }
        onPressAndHold: {
            contextMenu.popup()
        }

        Menu {
            id: contextMenu
            MenuItem {
                id: deleteMenu
                text: "Delete"

                onTriggered: {
                    session.messageListModel.deleteMessages()
                }
            }

            MenuItem {
                id: moveMenu
                text: "Move"

                //                enabled: Object.keys(accountObj).length === 1
                onTriggered: {

                    for (let key in accountObj) {
                        console.log(key)
                        selectedAccount = key
                    }

                    folderListModal.accountName = selectedAccount
                    folderListModal.open()
                }
            }
        }
    }

    FolderListModal {
        id: folderListModal
    }

    function diffHours(dt2, dt1) {
        var diff = (dt2.getTime() - dt1.getTime()) / 1000
        diff /= (60 * 60)
        return Math.abs(Math.round(diff))
    }

    function diffDate(first, second) {
        const oneDay = 24 * 60 * 60 * 1000
        return Math.round(Math.abs((first - second) / oneDay))
    }

    function parseDate(datetime) {
        var currentDate = new Date()

        var hourDiff = diffHours(datetime, currentDate)
        var dateDiff = diffDate(datetime, currentDate)

        if (dateDiff < 1) {
            return datetime.toLocaleTimeString(Qt.locale(), "h:m AP")
        } else if (dateDiff < 2) {
            return "Yesterday"
        } else if (dateDiff < 7) {
            return datetime.toLocaleDateString(Qt.locale(), "ddd")
        } else {
            return datetime.toLocaleDateString(Qt.locale(), "MMM d")
        }
    }

    Component.onCompleted: {
        for (var i = 0; i < session.accountEmail(); i++) {
            accountObj[session.accountEmail()[i]] = 0
        }
    }
}
