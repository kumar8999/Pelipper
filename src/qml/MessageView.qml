import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.15 as Controls
import org.kde.kirigami 2.17 as Kirigami
import QtWebView 1.15

Kirigami.Page {

    Kirigami.Theme.colorSet: Kirigami.Theme.View
    padding: 0

    header: Controls.Pane {
        visible: session.hasMsgLoaded
        Kirigami.Theme.colorSet: Kirigami.Theme.Window

        ColumnLayout {

            Kirigami.FormLayout {
                Layout.leftMargin: Kirigami.Units.gridUnit

                Controls.Label {
                    id: fromLabel
                    Kirigami.FormData.label: qsTr("From:")
                }

                Controls.Label {
                    id: toLabel
                    Kirigami.FormData.label: qsTr("To:")
                }

                Controls.Label {
                    id: dateLabel
                    Kirigami.FormData.label: qsTr("Date:")
                }

                Controls.Label {
                    id: subjectLabel
                    Kirigami.FormData.label: qsTr("Subject:")
                }
            }
        }
    }

    WebView {
        visible: session.hasMsgLoaded
        id: msgWebView
        Layout.fillWidth: true
        anchors.fill: parent
    }

    Kirigami.Heading {
        visible: !session.hasMsgLoaded
        level: 2
        anchors.centerIn: parent
        text: "No Message Selected"
    }
    function onMessageReady() {
        fromLabel.text = session.messageItem.from
        toLabel.text = session.messageItem.to
        dateLabel.text = session.messageItem.datetime.toLocaleDateString()
        subjectLabel.text = session.messageItem.subject
        msgWebView.loadHtml(session.messageItem.html, "")
    }

    Component.onCompleted: {
        session.messageItemChanged.connect(onMessageReady)
    }
}
