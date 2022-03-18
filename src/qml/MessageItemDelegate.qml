import QtQuick 2.0
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import org.kde.kirigami 2.13 as Kirigami

Kirigami.BasicListItem {
    id: messageDelegate

    property string subject
    property string sender
    property string dateString
    property bool highlightItem
    property bool seenflag
    property bool recentflag
    property string email

    signal itemClicked(var mouse)

    text: sender
    subtitle: subject
    trailing: Label {
        text: qsTr(dateString)
    }

    fadeContent: seenflag

    MouseArea {
        anchors.fill: parent

        onClicked: messageDelegate.itemClicked(mouse)
    }

    highlighted: highlightItem
}
