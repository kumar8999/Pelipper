import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.15 as Controls
import org.kde.kirigami 2.13 as Kirigami
import QtQuick.Controls.Styles 1.4

Kirigami.Page {
    property string serviceProvider: ""

    id: addSheet
    title: qsTr("Add Account")

    footer: Controls.DialogButtonBox {
        Controls.Button {
            text: qsTr("Ok")
            onClicked: {
                addAccount()
            }
        }

        Controls.Button {
            text: qsTr("Cancel")
            onClicked: {
                nameField.text = ""
                emailField.text = ""
                passwordField.text = ""
                imapServerField.text = ""
                imapPortField.text = ""
                smtpServerField.text = ""
                smtpPortField.text = ""
                close()
                inlineMessage.visible = false
            }
        }
    }

    ColumnLayout {

        Kirigami.InlineMessage {
            id: inlineMessage
            Layout.fillWidth: true
            text: ""
            visible: false
        }

        Controls.TextField {
            id: nameField
            placeholderText: qsTr("Enter Username")
        }

        Controls.TextField {
            id: emailField
            echoMode: TextInput.Normal
            placeholderText: qsTr("Enter Email")
        }

        Controls.TextField {
            id: passwordField
            echoMode: TextInput.PasswordEchoOnEdit
            placeholderText: qsTr("Enter Password ")

            Controls.ToolButton {
                anchors.right: parent.right
                icon.name: qsTr("password-show-on")

                onClicked: {
                    if (checked) {
                        passwordField.echoMode = TextInput.Normal
                    } else {
                        passwordField.echoMode = TextInput.PasswordEchoOnEdit
                    }
                }
            }
        }

        Controls.ComboBox {
            id: serviceProviderField
            model: ["Gmail", "Others"]
            onActivated: {
                serviceProvider = currentText
                handleServiceProvider()
            }
            Component.onCompleted: {
                serviceProvider = "Gmail"
                handleServiceProvider()
            }
        }

        Controls.TextField {
            id: imapServerField
            visible: serviceProvider === "Others"
            Kirigami.FormData.label: qsTr("Imap Server:")
            placeholderText: qsTr("Enter Imap Server ")
        }

        Controls.TextField {
            id: imapPortField
            visible: serviceProvider === "Others"
            Kirigami.FormData.label: qsTr("Imap Port:")
            placeholderText: qsTr("Enter Imap Port ")
            inputMethodHints: Qt.ImhDigitsOnly
        }

        Controls.TextField {
            id: smtpServerField
            visible: serviceProvider === "Others"
            Kirigami.FormData.label: qsTr("Smtp Server:")
            placeholderText: qsTr("Enter Smtp Server ")
        }

        Controls.TextField {
            id: smtpPortField
            visible: serviceProvider === "Others"
            Kirigami.FormData.label: qsTr("Smtp Port:")
            placeholderText: qsTr("Enter Smtp port ")
            inputMethodHints: Qt.ImhDigitsOnly
        }
    }

    function handleServiceProvider() {
        if (serviceProvider === "Gmail") {
            imapServerField.text = "imap.gmail.com"
            imapPortField.text = "993"
            smtpServerField.text = "smtp.gmail.com"
            smtpPortField.text = "465"
        } else {
            imapServerField.text = ""
            imapPortField.text = ""
            smtpServerField.text = ""
            smtpPortField.text = ""
        }
    }

    function addAccount() {
        var name = nameField.text
        var email = emailField.text
        var password = passwordField.text
        var imapserver = imapServerField.text
        var imapport = imapPortField.text
        var smtpserver = smtpServerField.text
        var smtpport = smtpPortField.text
        inlineMessage.visible = true
        inlineMessage.text = "Connecting..."
        session.addAccount(name, email, password, imapserver, imapport,
                           smtpserver, smtpport)
    }

    function onConnectionSuccess(isSuccess) {
        if (isSuccess) {
            inlineMessage.visible = true
            inlineMessage.type = Kirigami.MessageType.Positive
            inlineMessage.text = "Connected Successfully"
            inlineMessage.type = Kirigami.MessageType.Information
            inlineMessage.text = "Trying to login"
        } else {
            inlineMessage.text = "Error Connecting"
            inlineMessage.visible = true
            inlineMessage.type = Kirigami.MessageType.Error
        }
    }

    function onLoginSuccess(isSuccess) {
        if (isSuccess) {
            inlineMessage.text = "Logged In Successfully"
            inlineMessage.visible = false
            addSheet.close()
            nameField.text = ""
            emailField.text = ""
            passwordField.text = ""
            imapServerField.text = ""
            imapPortField.text = ""
            smtpServerField.text = ""
            smtpPortField.text = ""
        } else {
            inlineMessage.text = "Error Login"
            inlineMessage.type = Kirigami.MessageType.Error
            inlineMessage.visible = true
        }
    }

    Component.onCompleted: {
        session.connectionSuccessFul.connect(onConnectionSuccess)
        session.loginSuccessFul.connect(onLoginSuccess)
    }
}
