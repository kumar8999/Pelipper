import QtQuick 2.15
import QtQuick.Layouts 1.12
import QtQuick.Window 2.15
import QtQuick.Controls 2.12 as Controls
import org.kde.kirigami 2.13 as Kirigami
import Qt.labs.platform 1.0
import mail.widgets 1.0

Window {

    id: composer
    width: 700
    height: 500
    title: qsTr("Composer")

    Controls.Page {
        anchors.fill: parent

        ColumnLayout {
            anchors.fill: parent

            RowLayout {
                Controls.Label {
                    text: qsTr("From")
                }

                Controls.ComboBox {
                    id: fromField
                    Layout.fillWidth: true
                    model: session.accountEmail()
                }
            }

            RowLayout {
                Controls.Label {
                    text: qsTr("To")
                }

                Controls.TextField {
                    id: toField
                    Layout.fillWidth: true
                }
            }

            RowLayout {
                Controls.Label {
                    text: qsTr("Bcc")
                }

                Controls.TextField {
                    id: bccField
                    Layout.fillWidth: true
                }
            }

            RowLayout {
                Controls.Label {
                    text: qsTr("cc")
                }

                Controls.TextField {
                    id: ccField
                    Layout.fillWidth: true
                }
            }

            RowLayout {
                Controls.Label {
                    text: qsTr("Subject")
                }

                Controls.TextField {
                    id: subjectField
                    Layout.fillWidth: true
                }
            }

            Controls.ScrollView {
                Layout.fillHeight: true
                Layout.fillWidth: true

                Controls.TextArea {
                    id: msgField
                    textFormat: Qt.RichText
                    wrapMode: TextEdit.Wrap
                    focus: true
                    selectByMouse: true
                    persistentSelection: true
                }
            }

            ListView {
                Layout.fillWidth: true
                height: 50
                Controls.ScrollBar.vertical: Controls.ScrollBar {}

                delegate: Kirigami.BasicListItem {
                    label: model.name
                    icon: model.portrait
                    trailing: Controls.Button {
                        icon.name: "delete"
                    }
                }
            }

            RowLayout {
                Controls.ToolButton {
                    id: italicBtn
                    icon.name: "format-text-italic"
                    focusPolicy: Qt.TabFocus
                    checkable: true
                    checked: document.italic
                    onClicked: document.italic = !document.italic
                }

                Controls.ToolButton {
                    id: boldBtn
                    icon.name: "format-text-bold"
                    focusPolicy: Qt.TabFocus
                    checkable: true
                    checked: document.bold
                    onClicked: document.bold = !document.bold
                }

                Controls.ToolButton {
                    id: underlineBtn
                    icon.name: "format-text-underline-symbolic"
                    focusPolicy: Qt.TabFocus
                    checkable: true
                    checked: document.underline
                    onClicked: document.underline = !document.underline
                }

                Controls.ToolButton {
                    icon.name: "format-text-strikethrough"
                }

                Controls.ToolButton {
                    icon.name: "format-list-unordered"
                }

                Controls.ToolButton {
                    icon.name: "format-list-ordered"
                }

                Controls.ToolButton {
                    icon.name: "insert-image"
                }

                Controls.ToolButton {
                    icon.name: "mail-forward-attachment"
                }

                Controls.ToolButton {
                    id: fontFamilyToolButton
                    icon.name: "font-select-symbolic"
                    font.bold: document.bold
                    font.italic: document.italic
                    font.underline: document.underline

                    onClicked: {
                        fontDialog.currentFont.family = document.fontFamily
                        fontDialog.currentFont.pointSize = document.fontSize
                        fontDialog.open()
                    }
                }

                Controls.ToolButton {
                    id: textColorButton
                    icon.name: "color-select-symbolic"
                    focusPolicy: Qt.TabFocus
                    onClicked: colorDialog.open()

                    Rectangle {
                        width: aFontMetrics.width + 3
                        height: 2
                        color: document.textColor
                        parent: textColorButton.contentItem
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.baseline: parent.baseline
                        anchors.baselineOffset: 6

                        TextMetrics {
                            id: aFontMetrics
                            font: textColorButton.font
                            text: textColorButton.text
                        }
                    }
                }
            }
        }

        footer: Controls.DialogButtonBox {
            Controls.Button {
                text: qsTr("Send")
                onClicked: {
                    session.sendMessage(
                                fromField.currentText, toField.text,
                                ccField.text, bccField.text, subjectField.text,
                                msgField.text,
                                session.attachmentListModel.AttachmentList)
                }
            }

            Controls.Button {
                text: qsTr("Cancel")
                onClicked: {
                    composer.close()
                }
            }
        }
    }

    FontDialog {
        id: fontDialog
        onAccepted: {
            document.fontFamily = font.family
            document.fontSize = font.pointSize
        }
    }

    ColorDialog {
        id: colorDialog
        currentColor: "black"
    }

    DocumentHandler {
        id: document
        document: msgField.textDocument
        cursorPosition: msgField.cursorPosition
        selectionStart: msgField.selectionStart
        selectionEnd: msgField.selectionEnd
        textColor: colorDialog.color
    }
}
