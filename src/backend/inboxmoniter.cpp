#include "inboxmoniter.h"

InboxMoniter::InboxMoniter(QObject *parent)
    : QObject{parent}
{
}

void InboxMoniter::addAccount(Account *account)
{
    ImapService *imapService = account->createService();

    imapService->idleStart("INBOX");
}
