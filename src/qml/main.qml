import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.15
import org.kde.kirigami 2.13 as Kirigami

Kirigami.ApplicationWindow {
    property string webhtml: ""

    id: root
    title: qsTr("Pelipper Mail")

    pageStack.initialPage: session.isAccountInit ? mainPage : addAccountPage

    menuBar: MenuBar {
        Menu {
            title: qsTr("&File")

            Action {
                text: qsTr("Add Account")
                onTriggered: {
                    applicationWindow().pageStack.layers.push(
                                "qrc:/qml/AddAccount.qml")
                }
            }

            Action {
                text: qsTr("Compose")
                onTriggered: {
                    applicationWindow().pageStack.layers.push(
                                "qrc:/qml/ComposerWidget.qml")
                }
            }

            Action {
                text: qsTr("Refresh")
                onTriggered: {

                }
            }

            Action {
                text: qsTr("&Quit")
                onTriggered: Qt.quit()
            }
        }

        Menu {
            title: qsTr("&Edit")
        }

        Menu {
            title: qsTr("&Help")
            Action {
                text: qsTr("&About")
            }
        }
    }

    Component {
        id: addAccountPage

        Kirigami.Page {

            actions.right: Kirigami.Action {
                displayComponent: BusyIndicator {
                    MouseArea {
                        anchors.fill: parent

                        onClicked: {
                            console.log("clicked")
                        }
                    }
                }
            }

            Button {
                anchors.centerIn: parent
                text: qsTr("Add Account")
                onClicked: {
                    applicationWindow().pageStack.layers.push(
                                "qrc:/qml/AddAccount.qml")
                }
            }
        }
    }

    Component {
        id: mainPage

        MainWindow {}
    }

    Component {
        id: loadingPage

        Kirigami.Page {

            BusyIndicator {
                anchors.centerIn: parent
            }
        }
    }
}
