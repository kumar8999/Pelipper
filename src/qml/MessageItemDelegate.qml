import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.12
import QtQml 2.0
import org.kde.kirigami 2.13 as Kirigami

Kirigami.BasicListItem {
    property string subject
    property string sender
    property string dateString
    property bool highlightItem
    signal clicked(var mouse)

    id: item_delegate

    label: sender
    subtitle: subject
    trailing: Label {
        text: dateString
    }

    highlighted: highlightItem

    MouseArea {
        id: item_mousearea
        anchors.fill: parent
        onClicked: {
            item_delegate.clicked(mouse)
        }
    }
}
