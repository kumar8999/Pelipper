import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.15 as Controls
import org.kde.kirigami 2.17 as Kirigami
import QtWebView 1.15

Kirigami.Page {

    Kirigami.Theme.colorSet: Kirigami.Theme.View

    header: Controls.Pane {
        visible: session.hasMsgInited
        Kirigami.Theme.colorSet: Kirigami.Theme.Window
        ColumnLayout {

            Kirigami.FormLayout {
                Layout.leftMargin: Kirigami.Units.gridUnit
                Controls.Label {
                    Kirigami.FormData.label: qsTr("From:")
                    text: session.message.from
                }
                Controls.Label {
                    Kirigami.FormData.label: qsTr("To:")
                    text: session.message.to
                }

                Controls.Label {
                    Kirigami.FormData.label: qsTr("Date:")
                    text: session.message.datetime.toLocaleDateString()
                }
                Controls.Label {
                    Kirigami.FormData.label: qsTr("Subject:")
                    text: session.message.subject
                }
            }
        }
    }

    WebView {
        visible: session.hasMsgInited
        id: webView
        Layout.fillWidth: true
        anchors.fill: parent

        function setHtml() {
            webView.loadHtml(session.message.content, "")
        }

        Component.onCompleted: {
            session.messageChanged.connect(setHtml)
        }
    }

    Kirigami.Heading {
        visible: !session.hasMsgInited
        level: 2
        anchors.centerIn: parent
        text: "No Message Selected"
    }
}
