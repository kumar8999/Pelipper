import QtQuick 2.0
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import org.kde.kirigami 2.13 as Kirigami

ItemDelegate {
    property string subject
    property string sender
    property string dateString
    property bool highlightItem
    signal clicked(var mouse)

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
        font.bold: true
        wrapMode: Text.WrapAnywhere
        anchors.leftMargin: 10
        width: parent.width
        enabled: false
        color: Kirigami.Theme.disabledTextColor
    }

    Label {
        id: subtitle
        anchors.top: title.bottom
        anchors.leftMargin: 10
        text: subject
        font.weight: Font.Light
        elide: Text.ElideRight
        width: parent.width
        color: Kirigami.Theme.disabledTextColor
    }

    Label {
        id: trailing
        text: dateString
        anchors.right: parent.right
        anchors.rightMargin: 10
        wrapMode: Text.WrapAnywhere
        color: Kirigami.Theme.disabledTextColor
    }

    MouseArea {
        id: item_mousearea
        anchors.fill: parent
        onClicked: {
            item_delegate.clicked(mouse)
        }
    }
}
