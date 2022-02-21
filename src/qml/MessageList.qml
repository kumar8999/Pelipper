import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.12
import org.kde.kirigami 2.17 as Kirigami

ListView {
    property int mulBegin: 0

    id: control
    model: session.messageListModel
    snapMode: ListView.SnapToItem
    headerPositioning: ListView.OverlayHeader
    highlightRangeMode: ListView.StrictlyEnforceRange
    ScrollBar.vertical: ScrollBar {}
    spacing: 5



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
        id: item_delegate_
        width: control.width

        highlightItem: model.Selected
        sender: model.Sender
        dateString: parseDate(model.Date)
        subject: model.Subject
        seenflag: model.Seen
        recentflag: model.Recent

        onDoubleClicked: {
            session.selectedMessage(model.Email, model.Uid)
        }

        onClicked: {
            if (mouse.modifiers & Qt.ControlModifier) {
                control.model.toggleSelected(model.index)
                control.mulBegin = index
            } else if (mouse.modifiers & Qt.ShiftModifier) {
                var i
                if (index > control.mulBegin) {
                    for (i = control.mulBegin; i <= index; i++) {
                        control.model.setSelected(i)
                    }
                } else {
                    for (i = index; i <= control.mulBegin; i++) {
                        control.model.setSelected(i)
                    }
                }
            } else {
                console.log("selected")

                control.model.clearSelections()
                control.model.toggleSelected(model.index)
                if (model.Selected)
                    control.mulBegin = index

                console.log(model.Selected)
            }

            //            var emailList = control.model.selectedMessageAccounts()

            //            if (emailList.length === 1) {
            //                moveMenu.enabled = true
            //                session.folderListModel.loadFolderList(emailList[0])
            //            } else {
            //                moveMenu.enabled = false
            //            }
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

                onTriggered: {
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
}
