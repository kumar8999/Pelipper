import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12 as Controls
import org.kde.kirigami 2.13 as Kirigami

Kirigami.OverlaySheet {
    property string serviceProvider: ""

    id: addSheet

    header: Kirigami.Heading {
        text: qsTr("Add Account")
    }

    Kirigami.FormLayout {

        Kirigami.InlineMessage {
            id: inlineMessage
            Layout.fillWidth: true
            text: ""
            visible: false
        }

        Controls.TextField {
            id: nameField
            Kirigami.FormData.label: qsTr("Username:")
            placeholderText: qsTr("Enter Username (required)")
            onAccepted: emailField.forceActiveFocus()
        }

        Controls.TextField {
            id: emailField
            Kirigami.FormData.label: qsTr("Email:")
            echoMode: TextInput.Normal
            placeholderText: qsTr("Enter Email (required)")
            onAccepted: passwordField.forceActiveFocus()
        }

        Controls.TextField {
            id: passwordField
            Kirigami.FormData.label: qsTr("Password:")
            placeholderText: qsTr("Enter Password (required)")
            onAccepted: {
                if (serviceProvider === "") {
                    addButton.forceActiveFocus()
                } else {
                    imapServerField.forceActiveFocus()
                }
            }
        }

        Controls.ComboBox {
            id: serviceProviderField
            Kirigami.FormData.label: qsTr("Service Provider")
            model: ["Gmail", "Others"]
            onActivated: {
                serviceProvider = currentText
                handleServiceProvider()
            }
        }

        Controls.TextField {
            id: imapServerField
            visible: serviceProvider === "Others"
            Kirigami.FormData.label: qsTr("Imap Server:")
            placeholderText: qsTr("Enter Imap Server (required)")
        }

        Controls.TextField {
            id: imapPortField
            visible: serviceProvider === "Others"
            Kirigami.FormData.label: qsTr("Imap Port:")
            placeholderText: qsTr("Enter Imap Port (required)")
            inputMethodHints: Qt.ImhDigitsOnly
        }

        Controls.TextField {
            id: smtpServerField
            visible: serviceProvider === "Others"
            Kirigami.FormData.label: qsTr("Smtp Server:")
            placeholderText: qsTr("Enter Smtp Server (required)")
        }

        Controls.TextField {
            id: smtpPortField
            visible: serviceProvider === "Others"
            Kirigami.FormData.label: qsTr("Smtp Port:")
            placeholderText: qsTr("Enter Smtp port (required)")
            inputMethodHints: Qt.ImhDigitsOnly
        }

        Controls.Button {
            id: addButton
            Layout.fillWidth: true
            text: qsTr("Add")
            onClicked: {
                addAccount()
                nameField.text = ""
                emailField.text = ""
                passwordField.text = ""
                imapServerField.text = ""
                imapPortField.text = ""
                smtpServerField.text = ""
                smtpPortField.text = ""
            }
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
        //        session.createAccount("kumar", "darkknightbegins2@gmail.com",
        //                              "l1nux1sg00d", "imap.gmail.com", 993,
        //                              "smtp.gmail.com", 465)
        session.addAccount(name, email, password, imapserver, imapport,
                           smtpserver, smtpport)
    }

    function onErrConnectToServer() {
        inlineMessage.text = "Error Connecting"
        inlineMessage.visible = true
        inlineMessage.type = Kirigami.MessageType.Error
    }

    function onErrLogin() {
        inlineMessage.text = "Error Login"
        inlineMessage.type = Kirigami.MessageType.Error
        inlineMessage.visible = true
    }

    function onConnectionSuccess() {
        inlineMessage.visible = true
        inlineMessage.type = Kirigami.MessageType.Positive
        inlineMessage.text = "Connected Successfully"
        inlineMessage.type = Kirigami.MessageType.Information
        inlineMessage.text = "Trying to login"
    }

    function onLoginSuccess() {
        inlineMessage.text = "Logged In Successfully"
        inlineMessage.visible = false
        addSheet.close()
    }

    Component.onCompleted: {

        //                session.errorConnectingtoServer.connect(onErrConnectToServer)
        //                session.errorLogin.connect(onErrLogin)
        //                session.loginSuccess.connect(onLoginSuccess)
    }
}
