import sys
import datetime
from telethon import TelegramClient, events, sync

# These example values won't work. You must get your own api_id and
# api_hash from https://my.telegram.org, under API Development.
# 1 - api_id
api_id = 517469
if len(sys.argv) > 1:
  api_id = int(sys.argv[1])
# 2 - api_hash
api_hash = 'db10ed08e544033eda6fe4cd9ba2c889'
if len(sys.argv) > 2:
  api_hash = 'db10ed08e544033eda6fe4cd9ba2c889'
# 3 - channel_id_src
#VIP Broker: 1120145527
#test_fds_source_redirect: 1269847629
channel_id_src=1269847629
if len(sys.argv) > 3:
  channel_id_src=int(sys.argv[3])
# 4 - channel_id_dst
#channel_signals: 1241024177
#channel_signals_all: 1218158012
#test_fds: 1124412374
channel_id_dst=1124412374
if len(sys.argv) > 4:
  channel_id_dst=int(sys.argv[4])

print('PY ' + str(datetime.datetime.now().time()) + ': START FORWARDER TELEGRAM')
print('PY   api_id         : ' + str(api_id))
print('PY   api_hash       : ' + str(api_hash))
print('PY   channel_id_src : ' + str(channel_id_src))
print('PY   channel_id_dst : ' + str(channel_id_dst))
client = TelegramClient('session_name', api_id, api_hash)
client.start()

#print(client.get_me().stringify())

@client.on(events.NewMessage(pattern='(?i).*CONFIRM$|.*GO$|.*NO$|.*OPTION$'))
async def handler(event):
  if event.to_id.channel_id == channel_id_src:    
    print('PY ' + str(datetime.datetime.now().time()) + ': ' + str(event.message.message))
    await client.send_message(channel_id_dst, event.message.message)

#print('PY (Press Ctrl+C to stop this)')
client.run_until_disconnected()
