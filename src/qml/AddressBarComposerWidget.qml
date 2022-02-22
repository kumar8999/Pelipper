import QtQuick 2.15
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12 as Controls
import org.kde.kirigami 2.13 as Kirigami

ColumnLayout {
    property var addressLists: []
    property bool showAddressListsView: false
    property bool isToField: false

    Flow {
        Repeater {
            model: addressModel

            Rectangle {
                color: Kirigami.Theme.highlightColor
                radius: 50
                width: t_metrics.tightBoundingRect.width + 25
                height: t_metrics.tightBoundingRect.height + 5

                Row {
                    Text {
                        id: a_text
                        text: model.email
                        anchors.leftMargin: 5
                    }

                    TextMetrics {
                        id: t_metrics
                        font: a_text.font
                        text: a_text.text
                    }

                    Controls.ToolButton {
                        icon.name: qsTr("close-symbolic")
                    }
                }
            }
        }
    }

    ListModel {
        id: addressModel
    }

    RowLayout {
        Controls.TextField {
            id: addressField
            Layout.fillWidth: true

            onTextEdited: {
                if (addressField.text.charAt(
                            addressField.text.length - 1) === " ") {
                    if (validateEmail(addressField.text.trim())) {
                        addressModel.append({
                                                "email": addressField.text.trim(
                                                             )
                                            })
                        addressField.text = ""
                    } else {
                        inlineMessage.setText("Enter Valid Email")
                        addressField.activeFocus()
                        addressField.text = addressField.text.trim()
                    }
                }

                if (addressModel.count > 0) {
                    showAddressListsView = true
                } else {
                    showAddressListsView = false
                }
            }
        }

        Controls.ToolButton {
            id: showCcField
            visible: isToField
            text: qsTr("cc")
            onClicked: {
                if (composer.showCc) {
                    composer.showCc = false
                } else {
                    composer.showCc = true
                }
            }
        }
    }
}
