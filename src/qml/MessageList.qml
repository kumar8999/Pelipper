import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.12
import org.kde.kirigami 2.17 as Kirigami

ListView {
    property int mulBegin: 0

    id: control
    model: session.messageListModel
    headerPositioning: ListView.OverlayHeader

    header: TextField {
        width: parent.width
        placeholderText: qsTr("Search...")
    }

    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent
        width: parent.width

        visible: session.messageListModel.loading
        text: "loading..."
    }

    delegate: MessageItemDelegate {
        id: item_delegate
        width: control.width

        highlightItem: model.Selected
        sender: model.Sender
        dateString: model.Date
        subject: model.Subject

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
                control.model.clearSelections()
                control.model.toggleSelected(model.index)
                if (model.Selected)
                    control.mulBegin = index
                session.selectedMessage(model.Email, model.Uid)
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
                text: "Delete"

                onTriggered: {
                    session.messageListModel.deleteMessages()
                }
            }
        }
    }
}
