import QtQuick 2.15
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12 as Controls
import org.kde.kirigami 2.13 as Kirigami

ColumnLayout {
    property var addressLists: []
    property bool showAddressListsView: false
    property bool isToField: false
    property string fieldLabel: ""

    ListModel {
        id: addressModel
    }

    RowLayout {
        Layout.fillWidth: true
        height: parent.height

        Controls.Label {
            text: qsTr(fieldLabel)
        }

        Controls.TextField {
            id: addressField
            height: parent.height

            onEditingFinished: {
                if (validateEmail(addressField.text.trim())) {
                    addressModel.append({
                                            "email": addressField.text.trim()
                                        })
                    addressField.text = ""
                } else {
                    inlineMessage.setText("Enter Valid Email")
                    addressField.activeFocus()
                    addressField.text = addressField.text.trim()
                }

                if (addressModel.count > 0) {
                    showAddressListsView = true
                } else {
                    showAddressListsView = false
                }
            }
        }

        ListView {
            id: showAddressListsView
            Layout.fillWidth: true
            height: parent.height
            orientation: ListView.Horizontal
            Controls.ScrollBar.horizontal: Controls.ScrollBar {}

            model: addressModel
            delegate: Kirigami.BasicListItem {
                width: 200
                text: qsTr(model.email)
                trailing: Controls.ToolButton {
                    text: qsTr("x")
                }
            }
        }
    }

    function validateEmail(email) {
        let regex = /^(([^<>()[\]\\.,;:\s@"]+(\.[^<>()[\]\\.,;:\s@"]+)*)|(".+"))@((\[[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\])|(([a-zA-Z\-0-9]+\.)+[a-zA-Z]{2,}))$/
        let matches = email.toLowerCase().match(regex)
        return matches && matches.length > 0
    }
}
