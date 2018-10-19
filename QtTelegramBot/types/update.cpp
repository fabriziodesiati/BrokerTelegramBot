#include "update.h"

using namespace Telegram;

Update::Update(QJsonObject update)
{
    id = update.value("update_id").toInt();
    //FDS CHANGE
    //before
    //message = Message(update.value("message").toObject());
    //after
    message = Message(update.value("channel_post").toObject());
    if (update.contains("callback_query")) {
        callbackquery = CallbackQuery(update.value("callback_query").toObject());
    }
}
