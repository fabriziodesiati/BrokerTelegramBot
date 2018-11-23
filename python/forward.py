from telethon import TelegramClient, events, sync
import datetime

# These example values won't work. You must get your own api_id and
# api_hash from https://my.telegram.org, under API Development.
api_id = 517469
api_hash = 'db10ed08e544033eda6fe4cd9ba2c889'
#Channels ID:
#VIP Broker: 1120145527
#test_fds_source_redirect: 1269847629
channel_id_src=1269847629
#channel_signals: 
#channel_signals_all:
#test_fds: 1124412374
channel_id_dst=1124412374

print('PY ' + str(datetime.datetime.now().time()) + ': START FORWARED TELEGRAM')
client = TelegramClient('session_name', api_id, api_hash)
client.start()

#print(client.get_me().stringify())

@client.on(events.NewMessage(pattern='(?i).*CONFIRM$|.*GO$|.*NO$|.*STATUS$'))
async def handler(event):
  if event.to_id.channel_id == channel_id_src:    
    print('PY ' + str(datetime.datetime.now().time()) + ': ' + str(event.message.message))
    await client.send_message(channel_id_dst, event.message.message)

#print('PY (Press Ctrl+C to stop this)')
client.run_until_disconnected()
