#ifndef MESSAGE_H
#define MESSAGE_H

#include "contact.h"
#include "flags.h"

#include <libetpan/libetpan.h>

#include <QDateTime>
#include <QObject>

class BodyPart
{
public:
    QString m_TextHtml;
    QString m_PlainText;
    bool m_IsAttachment;
    QByteArray m_Payload;
    QString m_Filename;
    QString m_Charset;
    QString m_MimeType;
    int m_Size;
};

class Message : public QObject
{
    Q_OBJECT

public:
    explicit Message(QString accountEmail, ssize_t uid, QObject *parent = nullptr);
    Message(ssize_t uid, QObject *parent = nullptr);

    void setHeaderData(QString data);

    void setBodyData(QString data);

    ssize_t Uid() const;
    void setUid(ssize_t newUid);

    const QString &AccountEmail() const;
    void setAccountEmail(const QString &newAccountEmail);

    const QDateTime &DateTime() const;

    const QString &Subject() const;

    Contact *From() const;

    const QString &MsgID() const;

    const QList<Contact *> &To() const;

    const QList<Contact *> &cc() const;

    const QList<Contact *> &Bcc() const;

    const QList<Contact *> &ReplyTo() const;

    bool hasAttachment();

    int attachmentCount();

    const QString &Html() const;

    const QString &PlainText() const;

    const QString &Data() const;

    Flags *flags() const;
    void setFlags(Flags *newFlags);

private:
    void parseHeader();
    Contact *parseFrom(mailimf_from *mbList);
    QList<Contact *> parseTo(mailimf_to *mbList);
    QList<Contact *> parseMailBoxList(mailimf_mailbox_list *mbList);
    QList<Contact *> parseAddressList(mailimf_address_list *addressList);
    QList<Contact *> parseAddress(mailimf_address *addr);

    QString getMimeType(mailmime_content *content);
    QString getDiscreteMimeType(mailmime_discrete_type *type);
    QString getCompositeMimeType(mailmime_composite_type *type);

    void parseBody();
    void parseMime(mailmime *mime, int depth);
    void parseSingleMime(mailmime *mime, const QString &mimeType);
    void parseMimeFields(mailmime *mime, QString &filename, QString &charset, bool &isattachment);
    void parseMimeMessage(mailmime *mime, QString mimeType, int depth);

    void parseHtml();
    void parsePlainText();

    QString mimeToUtf8(std::string str);

private:
    QString m_AccountEmail;
    QString m_HeaderData;
    QString m_Data;

    ssize_t m_Uid;
    QDateTime m_DateTime;
    QString m_Subject;
    QString m_MsgID;
    Contact *m_From;
    QList<Contact *> m_To;
    QList<Contact *> m_cc;
    QList<Contact *> m_Bcc;
    QList<Contact *> m_ReplyTo;

    Flags *m_Flags;

    QString m_Html;
    QString m_PlainText;
    QList<BodyPart *> m_BodyParts;
};
#endif // MESSAGE_H
