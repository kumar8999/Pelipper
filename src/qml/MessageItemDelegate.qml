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
    height: 40

    Rectangle {
        anchors.fill: parent
        visible: highlightItem
        color: Kirigami.Theme.highlightColor
        opacity: 0.5
    }

    Label {
        id: title
        text: sender
        font.bold: seenflag ? false : true
        wrapMode: Text.WrapAnywhere
        anchors.leftMargin: 10
        width: parent.width
        enabled: false
    }

    Label {
        id: subtitle
        anchors.top: title.bottom
        anchors.leftMargin: 10
        text: subject
        font.bold: seenflag ? false : true
        elide: Text.ElideRight
        width: parent.width
    }

    Label {
        id: trailing
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
