import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.15
import org.kde.kirigami 2.13 as Kirigami
import QtWebView 1.15

Kirigami.Page {
    SplitView {
        anchors.fill: parent
        orientation: Qt.Horizontal

        SideBar {
            SplitView.minimumWidth: Kirigami.Units.gridUnit * 10
            SplitView.maximumWidth: Kirigami.Units.gridUnit * 15
        }

        MessageList {
            id: messageList
            SplitView.minimumWidth: Kirigami.Units.gridUnit * 15
            SplitView.maximumWidth: Kirigami.Units.gridUnit * 20
        }

        MessageView {
            Layout.fillWidth: true
        }
    }
}
