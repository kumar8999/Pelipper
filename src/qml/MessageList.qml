import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.12
import org.kde.kirigami 2.17 as Kirigami

ListView {
    property int mulBegin: 0

    id: messageListView
    model: session.messageListModel
    snapMode: ListView.SnapToItem
    headerPositioning: ListView.OverlayHeader
    highlightRangeMode: ListView.StrictlyEnforceRange
    ScrollBar.vertical: ScrollBar {}

    section.property: "size"
    section.criteria: ViewSection.FullString
    section.delegate: sectionHeading

    Component {
        id: sectionHeading
        Rectangle {
            width: container.width
            height: childrenRect.height
            color: "lightsteelblue"

            Text {
                text: "section"
                font.bold: true
                font.pixelSize: 20
            }
        }
    }

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

        onDoubleClicked: {
            messageListView.currentIndex = index
            session.messageListModel.setSeenFlag(index)
            session.selectedMessage(model.Email, model.Uid)
        }

        onClicked: {
            if (mouse.modifiers & Qt.ControlModifier) {
                messageListView.model.toggleSelected(model.index)
                messageListView.mulBegin = index
            } else if (mouse.modifiers & Qt.ShiftModifier) {
                var i
                if (index > messageListView.mulBegin) {
                    for (i = messageListView.mulBegin; i <= index; i++) {
                        messageListView.model.setSelected(i)
                    }
                } else {
                    for (i = index; i <= messageListView.mulBegin; i++) {
                        messageListView.model.setSelected(i)
                    }
                }
            } else {
                messageListView.model.clearSelections()
                messageListView.model.toggleSelected(model.index)
                if (model.Selected)
                    messageListView.mulBegin = index
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
