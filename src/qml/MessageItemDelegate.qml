import QtQuick 2.0
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import org.kde.kirigami 2.13 as Kirigami

ItemDelegate {
    property string subject
    property string sender
    property string dateString
    property bool highlightItem
    property bool seenflag
    property bool recentflag
    signal clicked(var mouse)
    signal doubleClicked

    id: item_delegate
    height: 45

    Rectangle {
        anchors.fill: parent
        Kirigami.Theme.colorSet: Kirigami.Theme.View
        radius: 5

        Rectangle {
            anchors.fill: parent
            visible: highlightItem
            color: Kirigami.Theme.highlightColor
            opacity: 0.5
            radius: 5
        }

        Label {
            id: title
            x: 10
            y: 3
            text: sender
            font.bold: seenflag ? false : true
            wrapMode: Text.WrapAnywhere
            width: parent.width
            enabled: false
        }

        Label {
            id: subtitle
            x: 10
            y: 3
            anchors.top: title.bottom
            text: subject
            font.bold: seenflag ? false : true
            elide: Text.ElideRight
            width: parent.width - 10
        }

        Label {
            id: trailing
            y: 3
            text: dateString
            anchors.right: parent.right
            anchors.rightMargin: 10
            wrapMode: Text.WrapAnywhere
        }

        MouseArea {
            id: item_mousearea
            anchors.fill: parent

            onClicked: {
                item_delegate.clicked(mouse)
            }

            onDoubleClicked: {
                item_delegate.doubleClicked()
            }
        }
    }
}
